<?php

/**
 * @file
 * Define a new StateMachine for the node
 */

/**
 * Implements hook_state_flow_plugins().
 *
 * Define the ctools plugin to add a new state machine type for the node workflow.
 * In this example we are Add a "reviewed" state to the StateFlow class.
 */

/**
 * Implements hook_state_flow_plugins().
 */
function hook_state_flow_plugins() {
  $info = array();
  $path = drupal_get_path('module', 'state_flow') . '/plugins';
  $info['state_flow_test'] = array(
    'handler' => array(
      'class' => 'StateFlowTest',
      'file' => 'state_flow.inc',
      'path' => $path,
      'parent' => 'state_flow'
    ),
  );
  return $info;
}

/**
 * Define a new workflow for a node type
 */
class StateFlowTest extends StateFlow {
	/**
	 * Override the init method to set the new states
	 *
	 * Add a to review state and "Review" event
	 */
	public function init() {
	  sefl::init();
		$this->add_state('review');

    $this->create_event('review', array(
      'origin' => 'draft',
      'target' => 'review',
    ));

	  // Initialize events
	  $this->create_event('publish', array(
	    'origin' => 'review',
	    'target' => 'published',
	  ));
	}
}