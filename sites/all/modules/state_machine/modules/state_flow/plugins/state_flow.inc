<?php

/**
 * @file
 * State Flow implementation of the State Machine class
 */

state_machine_load_class_file();

class StateFlow extends StateMachine {
  /**
   * Called from StateMachine::__construct to initialize the states and events.
   * Define two states.
   * First revision:
   *  - Expose go to draft button
   *  - Expose go to publish button
   *  - Upon publish, create new revision (handled outside of state machine)
   * Second revision:
   *  - Menu alter edit link to load most recent revision (whether published or revision from states)
   *  - On hook_nodeapi (op: load), force new revision checkbox on node edit form
   *    - Expose go to draft button
   *  - Create new revision, prevent node table from updating new revision as published revision
   *  - Expose go to publish button
   *  - Upon publish, set revision id in node table
   *  - Repeat as needed
   */
  public function init() {
    // Initialize states
    $this->create_state('draft');
    $this->create_state('published', array(
      'on_enter' => array($this, 'on_enter_published'),
      'on_exit' => array($this, 'on_exit_published'),
    ));
    $this->create_state('unpublished');

    // Initialize events
    $this->create_event('publish', array(
      'origin' => 'draft',
      'target' => 'published',
    ));
    $this->create_event('unpublish', array(
      'origin' => 'published',
      'target' => 'unpublished',
      'permission' => 'publish and unpublish content',
    ));
    $this->create_event('to draft', array(
      'origin' => 'unpublished',
      'target' => 'draft',
    ));
  }

  public function on_enter_published() {
    $this->set_published();
    $this->set_node_revision();
    $this->set_principle_revision();
  }

  public function on_exit_published() {
    $this->set_unpublished();
  }

  public function get_event($key) {
    if (!array_key_exists($key, $this->events)) {
      return FALSE;
    }

    if (is_array($this->events[$key])) {
      $options = $this->events[$key];
      $this->events[$key] = new StateFlow_Event($this, $options);
    }

    return $this->events[$key];
  }

  public function get_object() {
    return $this->object;
  }

  public function get_states_options() {
    return $this->states;
  }

  /**
   * Extending fire_event() from state_machine's base.inc to add uid and log
   * arguments.
   */
  public function fire_event($key, $uid = NULL, $log = '') {
    $event = $this->get_event($key);

    if ($event && ($new_state = $event->execute())) {
      // Allow the previous state to run its 'on_exit' callbacks.
      $this->get_state($this->get_current_state())->on_exit();

      // Set and save the new state.
      $this->set_current_state($new_state);
      $this->persist();

      // Write a history record for this state change.
      if (empty($uid)) {
        global $user;
        $uid = $user->uid;
      }
      $this->write_history($uid, $log);

      // Allow the new state to run its 'on_enter' callbacks.
      $this->get_state($this->get_current_state())->on_enter();

      // Allow the event to "finish".
      $event->finish();

      // Allow state_flow to provide other hooks or event triggers.
      state_flow_invoke_event_handlers($this->object, $new_state);
    }
    else {
      $this->on_event_fail($event);
      return FALSE;
    }
  }

  public function write_history($uid, $log = '') {
    $data = new stdClass();
    $data->vid = $this->object->vid;
    $data->nid = $this->object->nid;
    $data->state = $this->get_current_state();
    $data->timestamp = REQUEST_TIME;
    $data->uid = $uid;
    $data->log = $log;
    return drupal_write_record('node_revision_states_history', $data);
  }

  public function persist() {
    $vid = $this->object->vid;
    $nid = $this->object->nid;

    $data = new stdClass();
    $data->vid = $vid;
    $data->nid = $nid;
    $data->state = $this->get_current_state();
    $data->timestamp = REQUEST_TIME;
    $data->status = 1;

    $update = ($this->existing_revision($nid, $vid)) ? array('vid') : array();
    return drupal_write_record('node_revision_states', $data, $update);
  }

  public function load() {
    $state = FALSE;
    if (!empty($this->object->vid)) {
      $state = $this->revision_state($this->object->vid);
    }
    elseif (!empty($this->object->nid)) {
      $state = $this->latest_state($this->object->nid);
    }
    return $state;
  }

  public function set_published() {
    $this->object->status = 1;

    // Ensure that all other published revisions are unpublished
    $published_revs = db_query('
      SELECT vid
      FROM {node_revision_states}
      WHERE nid = :nid
      AND vid <> :vid
      AND state = :state
      ORDER BY vid DESC',
      array(
        ':nid' => $this->object->nid,
        ':vid' => $this->object->vid,
        ':state' => 'published',
      ))
      ->fetchAll();
    if (is_array($published_revs)) {
      foreach ($published_revs as $rev) {
        $rev_vid = isset($rev->vid) ? intval($rev->vid) : FALSE;
        $rev_node = $rev_vid ? node_load($this->object->nid, $rev_vid) : FALSE;
        if ($rev_node) {
          $rev_state = state_flow_load_state_machine($rev_node, TRUE);
          $rv = $rev_state->fire_event('unpublish', NULL, t('Unpublished due to the publication of revision @vid.', array('@vid' => $this->object->vid)));
        }
      }
    }

    // Set the published status on the node_revision record for this revision
    $res = db_update('node_revision')
      ->fields(array(
        'status' => $this->object->status,
      ))
      ->condition('vid', $this->object->vid)
      ->execute();

    // Set the published status on the node record for this node
    $res = db_update('node')
      ->fields(array(
        'status' => $this->object->status,
      ))
      ->condition('nid', $this->object->nid)
      ->execute();

    // Update the node access table for this node.
    $node = node_load($this->object->nid, $this->object->vid, TRUE);
    node_access_acquire_grants($node);

    //rebuild taxonomy index
    if (function_exists('taxonomy_delete_node_index')) {
      taxonomy_delete_node_index($node);
    }
    if (function_exists('taxonomy_build_node_index')) {
      taxonomy_build_node_index($node);
    }
  }

  public function set_unpublished() {
    $this->object->status = 0;

    // Set the unpublished status on the node_revision record for this revision
    $res = db_update('node_revision')
      ->fields(array(
        'status' => $this->object->status,
      ))
      ->condition('vid', $this->object->vid)
      ->execute();

    // If the revision to unpublish is the latest published revision for the
    // node, then unpublish the node itself
    if ($this->object->vid == $this->get_latest_revision($this->object->nid)) {
      // Set the published status on the node record for this node
      $res = db_update('node')
        ->fields(array(
          'status' => $this->object->status,
        ))
        ->condition('nid', $this->object->nid)
        ->execute();
    }

    // Update the node access table for this node.
    $node = node_load($this->object->nid, $this->object->vid, TRUE);
    node_access_acquire_grants($node);
  }

  public function set_node_revision() {
    $vid = $this->get_latest_revision($this->object->nid);
    if (!empty($vid) && ($vid != $this->object->vid)) {
      $rev_state_rec = $this->revision_state_record($this->object->nid, $this->object->vid);
      state_flow_promote_node_revision($rev_state_rec, $this->object->nid, $this->object->vid);
    }
    $result = db_update('node_revision')
        ->fields(array(
          'status' => 1,
        ))
        ->condition('vid', $vid)
        ->execute();
  }

  public function set_principle_revision() {
    $nid = $this->object->nid;
    $vid = $this->get_latest_revision($nid);
    $result = db_update('node_revision_states')
      ->fields(array(
        'status' => 0,
      ))
      ->condition('nid', $nid)
      ->condition('vid', $vid, '!=')
      ->execute();
  }

  public function get_latest_revision($nid) {
    $result = db_query('SELECT MAX(vid) FROM {node_revision} WHERE nid = :nid', array(':nid' => $nid))->fetchCol('vid');
    return !empty($result[0]) ? $result[0] : FALSE;
  }

  public function existing_revision($nid, $vid) {
    $result = db_select('node_revision_states', 'nrs')
      ->fields('nrs')
      ->condition('vid', $vid)
      ->countQuery()
      ->execute()
      ->fetchAll();
    return ($result[0]->expression) ? TRUE : FALSE;
  }

  public function revision_state($vid) {
    $latest_state = db_query('
      SELECT state
      FROM {node_revision_states}
      WHERE vid = :vid
      LIMIT 0, 1',
      array(
        ':vid' => $vid
      ))
      ->fetchCol('state');
    return !empty($latest_state[0]) ? $latest_state[0] : FALSE;
  }

  public function revision_state_record($nid, $vid = NULL) {
    if (!empty($vid)) {
      $rev_state = db_query('
        SELECT *
        FROM {node_revision_states}
        WHERE vid = :vid
        LIMIT 0, 1',
        array(
          ':vid' => $vid
        ))
        ->fetchAll();
    }
    else {
      $rev_state = db_query('
        SELECT state
        FROM {node_revision_states}
        WHERE nid = :nid
        ORDER BY vid DESC
        LIMIT 0, 1',
        array(
          ':nid' => $nid
        ))
        ->fetchAll();
    }
    return isset($rev_state[0]->nid) ? $rev_state[0] : FALSE;
  }

  public function latest_state($nid) {
    $latest_state = db_query('
      SELECT state
      FROM {node_revision_states}
      WHERE nid = :nid
      AND status = 1
      AND vid = :vid
      ORDER BY timestamp DESC
      LIMIT 0, 1',
      array(
        ':nid' => $nid,
        ':vid' => $this->get_latest_revision($nid)
      ))
      ->fetchCol('state');
    return !empty($latest_state[0]) ? $latest_state[0] : FALSE;
  }
}

class StateFlow_Event extends StateMachine_Event {
  public function get_machine() {
    return $this->machine;
  }

  public function validate() {
    if (parent::validate()) {
      if (!empty($this->options['permission'])) {
        return user_access($this->options['permission']);
      }
      return TRUE;
    }
    return FALSE;
  }
}
