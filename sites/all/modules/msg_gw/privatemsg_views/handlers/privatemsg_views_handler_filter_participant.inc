<?php

/**
 * Participant filter to select user on behalf of who to view private message 
 * thread.
 * 
 * TODO: Maybe extend views_handler_filter_user_name instead ?
 */
class privatemsg_views_handler_filter_participant extends views_handler_filter {
  // exposed filter options
  var $no_single = TRUE;
  
  function init(&$view, $options) {
    parent::init($view, $options);
    // Set initial participant to the current user. This gets overriden inside 
    // accept_exposed_input().
    $this->value = $this->options['value'] = $GLOBALS['user']->uid;
    
    // Check user permission to select participant.
    if (!privatemsg_user_access('read all private messages')) {
      $this->options['exposed'] = FALSE;
    }
  }
  
  /**
   * Prepare options array for the participant select.
   */
  function participant_options($thread_id) {
    $options = array();   
    // Add participants.
    if (!empty($thread_id)) {
      $participants = privatemsg_views_get_participants($thread_id);
      if (is_array($participants[$thread_id])) {
        foreach($participants[$thread_id] as $account) {
          $options[$account->uid] = check_plain($account->name);
        }
      }
    }
    return $options;
  }
  
  /**
   * Provide a form for setting options.
   */
  function value_form(&$form, &$form_state) {
    global $user;
    $participant_options = $this->participant_options($this->view->args[0]);
    // Add current user.
    $participant_options[$user->uid] = check_plain($user->name);
    
    $form['value'] = array(
      '#type' => 'select',
      '#description' => t('Please select a user on behalf of who to view this thread.'),
      '#options' => $participant_options,
      '#default_value' => $this->value, 
    );
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];
      if (!isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }
    }
  }
  
  function option_definition() {
    global $user;
    $options = parent::option_definition();
    $options['value'] = array('default' => $GLOBALS['user']->uid);    
    $options['expose']['contains']['label'] = array(
      'default' => 'View the thread on behalf of', 
      'translatable' => TRUE,
    );
    return $options;
  }
  
  function expose_options() {
    $this->options['expose'] = array(
      'use_operator' => FALSE,
      'operator' => $this->options['id'] . '_op',
      'identifier' => $this->options['id'],
      'label' => 'View the thread on behalf of',
      'remember' => FALSE,
      'single' => TRUE,
      'optional' => FALSE,
    );
  }
  
  function admin_summary() {
    return t('Advanced participant selector');
  }
  
  function query() {
    $this->ensure_my_table();
    $participants = array_keys($this->participant_options($this->view->args[0]));
    // If current user is not a participant but a privileged user, add 
    // DISTINCT(mid) instead. Otherwise Views would return empty set.
    // Also this is useful to get full message list in situations such as 
    // when we filter out deleted messages but want to let privileged users 
    // see all messages including deleted ones. 
    // Message properties such as "New" mark are broken in this case though 
    // cause we have a list of messages from random participants of the thread.
    if (privatemsg_user_access('read all private messages') && !in_array($this->value, $participants)) {
      $this->query->set_distinct();
    }
    else {
      $this->query->add_where($this->options['group'], "$this->table_alias.$this->real_field = %d", $this->value);
    }
  }
}
