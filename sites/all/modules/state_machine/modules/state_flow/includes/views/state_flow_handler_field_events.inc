<?php

/**
 * Views field handler to display the available State Flow events for the
 * selected node revision.
 */
class state_flow_handler_field_events extends views_handler_field {
  function query() {
    $this->ensure_my_table();
  }

  function render($values) {
    $state_node = node_load($values->nid);
    $state_state_flow = state_flow_load_state_machine($state_node);
    $state_events = $state_state_flow->get_available_events();
    $state_actions = array();
    foreach ($state_events as $event) {
      if (state_flow_access($state_node, $event)) {
        $state_actions[] = l(drupal_ucfirst(t($event)), 'node/' . $state_node->nid . '/revisions/' . $state_node->vid . '/workflow/' . $event, array('query' => array('destination' => $_GET['q']))) . ' ';
      }
    }
    $state_actions_str = implode(' | ', $state_actions);
    return $state_actions_str;
  }
}
