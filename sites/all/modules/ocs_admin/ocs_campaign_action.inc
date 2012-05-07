<?php


function ocs_admin_campaign_action_form( $form, &$form_state, $campaign_id, $action_id = NULL)
{
    $form['body'] = array(
        '#type' => 'markup',
        '#markup' => t( 'Select the action to add.'),
    );

    $form['campaign_id'] = array(
        '#type' => 'hidden',
        '#value' => $campaign_id,
    );

	$selected_action = isset( $form_state['values']['action_id']) ? $form_state['values']['action_id'] : $action_id;

    $items = entity_load('node', FALSE, array('type' => 'ocs_actions'));
    $options = array();
    foreach( $items as $item) {
        $client = taxonomy_term_load( $item->field_ocs_client_id['und'][0]['tid']);
//        dpm( $client, 'client');
        $options[$client->name][$item->nid] = $item->title;
		if ( ! isset($selected_action)) $selected_action = $item->nid;
    }

	$form['action_id'] = array(
		'#type' => 'select',
		'#title' => t('Action to add'),
		'#options' => $options,
		'#description' => t('The Action shall be applied to the system.'),
		'#ajax' => array(
			'callback' => 'ocs_admin_ajax_action_param',
			'wrapper' => 'campaign_action_param_div',
			),
	);

	// get list of parameters
	$form['param'] = array(
		'#type' => 'fieldset',
		'#title' => 'Parameters',
		'#prefix' => '<div id="campaign_action_param_div">',
		'#suffix' => '</div>',
		);

	$action = node_load( $selected_action);
	foreach( $action->field_ocs_param_ref['und'] as $param) {
		$para = node_load( $param['target_id']);

		$key = $selected_action . ':' . $param['target_id'];
		$form['param'][$key] = array(
			'#type' => 'textfield',
			'#title' => $para->title,
		);
	}

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save configuration'),
    );

    return $form;
}


function ocs_admin_ajax_action_param( $form, &$form_state)
{
	return $form['param'];
}

function ocs_admin_campaign_action_form_submit( $form, &$form_state)
{
    dpm( $form_state['values'], 'form_state');
    $action = $form_state['values'];

    ocs_admin_campaign_action_save( $action);

    $form_state['redirect'] = 'ocs_admin/campaign/' . $action['campaign_id'] .'/rule';
}

/*
    save OCS campaign event
*/
function ocs_admin_campaign_action_save( $campaign)
{
    db_set_active( 'ocsdb');

    $fields = array();
	foreach( array( 'campaign_id', 'action_id') as $key) {
		$fields[$key] = $campaign[$key];
	}

    $status = db_merge( 'ocs_campaign_actions')
        ->key( array( 'action_id' => $campaign['action_id'], 'campaign_id' => $campaign['campaign_id']))
        ->fields( $fields)
        ->execute();
    dpm( $status, 'db_merge result');

    db_set_active( 'default');
}
 
/*
** delete an action
*/
function ocs_admin_campaign_action_delete_confirm( $form, &$form_state, $campaign_id, $action_id)
{
	$action_name = isset( $_GET['action_name']) ? $_GET['action_name'] : $action_id;

    $form['#campaign_id'] = $campaign_id;
    $form['#action_id'] = $action_id;
    $form['#action_name'] = $action_name;
    $caption = '<p>' . 
            t('<strong>Warning:</strong> The action "%key" will be deleted from the Campaign.',
                array( '%key' => $action_name)) .
            '</p>';

    $caption .= '<p>'. t('This action cannot be undone.') . '</p>';

    return confirm_form( $form, t('Are you sure to delete the action "%key"?',
        array( '%key' => $action_name)),
        'ocs_admin/campaign/' . $campaign_id . '/rule',
        $caption, t('Delete'));
}

function ocs_admin_campaign_action_delete_confirm_submit( $form, &$form_state)
{
    $campaign_id = $form['#campaign_id'];
    $action_id = $form['#action_id'];
    $action_name = $form['#action_name'];
    ocs_admin_campaign_delete_action( $campaign_id, $action_id);

    drupal_set_message( t('The event %key has been removed from the Campaign.', 
			array( '%key' => $action_name)));

    $form_state['redirect'] = 'ocs_admin/campaign/' . $campaign_id . '/rule';
}

function ocs_admin_campaign_delete_action( $campaign_id, $action_id)
{
    db_set_active( 'ocsdb');

	// delete event from campaign
	$result = db_delete( 'ocs_campaign_actions')
		->condition( 'campaign_id', $campaign_id, '=')
		->condition( 'action_id', $action_id, '=')
		->execute();
	dpm( $result, 'ocs_campaign_event result');

    db_set_active( 'default');
}


/*
   get action list
   */
function ocs_admin_campaign_actions_load_all( $campaign_id)
{
    db_set_active('ocsdb');

    // get group information
    $query = db_select( 'ocs_campaign_actions', 'c');
    $result = $query->condition( 'c.campaign_id', $campaign_id, '=')
        ->fields( 'c')
        ->execute()
        ->fetchAllAssoc( 'action_id', PDO::FETCH_ASSOC);
    

    db_set_active( 'default');

    return $result;
}

function ocs_admin_campaign_add_action_form( $form, &$form_state, $campaign_id = NULL, $action_id=NULL)
{
	$options = &drupal_static( __FUNCTION__ . 'options', array());
	$paramlist = &drupal_static( __FUNCTION__ . 'param', array());

	$cond = isset( $form_state['values']['cond']) ? $form_state['values']['cond'] : NULL;

	if ( empty( $options) || empty( $param)) {
		$events = ocs_admin_campaign_events_load_all( $campaign_id);
//		dpm( $events, 'events');

		foreach( array_keys($events) as $event_id) {
			$node = node_load( $event_id);
	//		dpm( $node, 'node');

			foreach( $node->field_ocs_param_ref['und'] as $param) {
				$para = node_load( $param['target_id']);
//				dpm( $para, 'parameter');

				// store select options
				$key = $event_id . ':' . $param['target_id'];
				$value = $node->title . ' : ' . $para->title;
				$options[ $node->title][ $key] = $value;
				// store parameter info
				$paramlist[$para->nid] = $para->field_ocs_param_type['und'][0]['value'];
				$paramlist[$key] = $value;
				if ( ! isset( $cond)) {
					$cond = $event_id . ':' . $param['target_id'];
				}

				// add additional condition if the parameter is 'account key' type
				if ( $para->field_ocs_param_type['und'][0]['value'] == OCS_PARAM_TYPE_ACCOUNT_KEY) {
					$options[ $node->title][ $key.'account'] = $value . ' ...';
					$paramlist[$para->nid.'account'] = OCS_PARAM_TYPE_ACCOUNT_KEY;
					// reset the type of original parameter as string
					$paramlist[$para->nid] = OCS_PARAM_TYPE_STRING;		
				}

			}
		}

		// get counter name list
		module_load_include( 'inc', 'ocs_admin', 'ocs_counter');
		$counter_list = ocs_admin_counter_name_load_all();
		$counters = array();
		foreach( $counter_list as $id => $value) {
			$counters[$id] = $value['counter_name'];
		}

	}

	if ( isset( $action_id)) {
		// if action_id is not NULL, it's editing mode
		$condition = ocs_admin_campaign_action_load( $action_id);
//		dpm( $condition, 'condition');
		$cond = $condition['event_id'].':'.$condition['param_id'];
	}

//	dpm( $options, 'options');
	$form = array();

	$form['#campaign_id'] = $campaign_id;
	$form['#paramlist'] = $paramlist;
	$form['#action_id'] = $action_id;

	$form['cond'] = array(
		'#type' => 'select',
		'#title' => t('Parameters'),
		'#options' => $options,
		'#default_value' => $cond,
		'#ajax' => array(
			'callback' => 'ocs_admin_ajax_condition_operator',
			'wrapper' => 'replace_operator_div',
			),
	);

	$form['container'] = array(
		'#type' => 'container',
		'#prefix' => '<div id="replace_operator_div">',
		'#suffix' => '</div>',
	);

	if ( substr( $cond, -7) == 'account') {
		$form['container']['counter'] = array(
			'#type' => 'select',
			'#title' => t('Counter'),
			'#options' => $counters,
		);
	}


	$form['container']['operator'] = array(
		'#type' => 'select',
		'#title' => 'Operator',
		'#options' => _campaign_operator( $paramlist, $cond),
		'#default_value' => isset($condition) ? $condition['operator'] : NULL,
	);

	$form['valueset'] = array(
		'#type' => 'fieldset',
		'#title' => 'Value',
		'#prefix' => '<div id="replace_value_div">',
		'#suffix' => '</div>',
	);

	$direct_input =  isset( $form_state['values']['value_type']) ? $form_state['values']['value_type'] : 1;
	$form['valueset']['value_type'] = array(
		'#type' => 'checkbox',
		'#title' => 'Input direct value',
		'#default_value' => $direct_input,
		'#ajax' => array(
			'callback' => 'ocs_admin_ajax_value_type',
			'wrapper' => 'replace_value_div',
		),
	);


	if ( $direct_input ) {
		$form['valueset']['value'] = array(
			'#type' => 'textfield',
			'#title' => 'Value',
			'#size' => 30,
			'#default_value' => isset($condition) ? $condition['value'] : NULL,
		);
	}
	else {
		$form['valueset']['value_list'] = array(
			'#type' => 'select',
			'#title' => 'Customer\'s Account Balance',
			'#options' => $counters,
		);
	}

	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save configuration'),
	);

	$form['actions']['cancel'] = array(
		'#type' => 'submit',
		'#value' => t('Cancel'),
	);

	return $form;
}


/*

function ocs_admin_ajax_condition_operator( $form, $form_state)
{
	return $form['container'];
}

function ocs_admin_ajax_value_type( $form, $form_state)
{
	return $form['valueset'];
}

*/


function ocs_admin_campaign_add_action_form_submit( $form, &$form_state)
{
	$values = $form_state['values'];
	dpm( $values, 'values submit');
	if ( $values['op'] != 'Cancel') {
		list( $event_id, $param_id) = explode( ':', $values['cond']);
		$cond = array(
			'action_id' => $form['#action_id'],
			'campaign_id' => $form['#campaign_id'],
			'event_id' => $event_id,
			'param_id' => $param_id,
			'param_name' => $form['#paramlist'][ $values['cond']],
			'operator' => $values['operator'],
			'value' => $values['value'],
			'action_name' => t( '@para @op @value', array( 
					'@para' =>  $form['#paramlist'][ $values['cond']],
					'@op' => $values['operator'],
					'@value' => $values['value'])),
		);
		ocs_admin_campaign_condition_save( $cond);
	}

	$form_state['redirect'] = 'ocs_admin/campaign/' . $form['#campaign_id'] . '/rule';
}

/*
    save OCS campaign condition
*/
/*
function ocs_admin_campaign_action_save( $cond)
{
    db_set_active('ocsdb');

	$result = db_merge( 'ocs_campaign_conditions')
		->key( array( 'action_id' => $cond['action_id']))
		->fields( $cond)
		->execute();
    dpm( $result, 'db_merge result');

    db_set_active( 'default');

	return $result;
}

*/

function ocs_admin_campaign_edit_action_form( $form, &$form_state, $campaign_id = NULL, $event_id=NULL)
{
	$options = &drupal_static( __FUNCTION__ . 'options', array());
	$paramlist = &drupal_static( __FUNCTION__ . 'param', array());

	$cond = isset( $form_state['values']['cond']) ? $form_state['values']['cond'] : NULL;

	if ( empty( $options) || empty( $param)) {
		$node = node_load( $event_id);
//		dpm( $node, 'node');

		foreach( $node->field_ocs_param_ref['und'] as $param) {
			$para = node_load( $param['target_id']);
//				dpm( $para, 'parameter');

			// store select options
			$key = $event_id . ':' . $param['target_id'];
			$value = $para->title;
			$options[ $node->title][ $key] = $value;
			// store parameter info
			$paramlist[$para->nid] = $para->field_ocs_param_type['und'][0]['value'];
			$paramlist[$key] = $value;
			if ( ! isset( $cond)) {
				$cond = $event_id . ':' . $param['target_id'];
			}

			// add additional condition if the parameter is 'account key' type
			if ( $para->field_ocs_param_type['und'][0]['value'] == OCS_PARAM_TYPE_ACCOUNT_KEY) {
				$options[ $node->title][ $key.'account'] = $value . ' ...';
				$paramlist[$para->nid.'account'] = OCS_PARAM_TYPE_ACCOUNT_KEY;
				// reset the type of original parameter as string
				$paramlist[$para->nid] = OCS_PARAM_TYPE_STRING;		
			}
		}

		// get counter name list
		module_load_include( 'inc', 'ocs_admin', 'ocs_counter');
		$counter_list = ocs_admin_counter_name_load_all();
		$counters = array();
		foreach( $counter_list as $id => $value) {
			$counters[$id] = $value['counter_name'];
		}

	}

	if ( isset( $cond_id)) {
		// if cond_id is not NULL, it's editing mode
		$condition = ocs_admin_campaign_cond_load( $cond_id);
//		dpm( $condition, 'condition');
		$cond = $condition['event_id'].':'.$condition['param_id'];
	}

//	dpm( $options, 'options');
	$form = array();

	$form['#campaign_id'] = $campaign_id;
	$form['#event_id'] = $event_id;
	$form['#paramlist'] = $paramlist;
	$form['#cond_id'] = $cond_id;

	$form['event'] = array(
		'#type' => 'markup',
		'#markup' => t( 'Event : <em>@name</em>', array('@name' => $_GET['event_name'])), 
	);

	$form['cond'] = array(
		'#type' => 'select',
		'#title' => t('Parameters'),
		'#options' => $options,
		'#default_value' => $cond,
		'#ajax' => array(
			'callback' => 'ocs_admin_ajax_condition_operator',
			'wrapper' => 'replace_operator_div',
			),
	);

	$form['container'] = array(
		'#type' => 'container',
		'#prefix' => '<div id="replace_operator_div">',
		'#suffix' => '</div>',
	);

	if ( substr( $cond, -7) == 'account') {
		$form['container']['counter'] = array(
			'#type' => 'select',
			'#title' => t('Counter'),
			'#options' => $counters,
		);
	}


	$form['container']['operator'] = array(
		'#type' => 'select',
		'#title' => 'Operator',
		'#options' => _campaign_operator( $paramlist, $cond),
		'#default_value' => isset($condition) ? $condition['operator'] : NULL,
	);

	$form['valueset'] = array(
		'#type' => 'fieldset',
		'#title' => 'Value',
		'#prefix' => '<div id="replace_value_div">',
		'#suffix' => '</div>',
	);

	$direct_input =  isset( $form_state['values']['value_type']) ? $form_state['values']['value_type'] : 1;
	$form['valueset']['value_type'] = array(
		'#type' => 'checkbox',
		'#title' => 'Input direct value',
		'#default_value' => $direct_input,
		'#ajax' => array(
			'callback' => 'ocs_admin_ajax_value_type',
			'wrapper' => 'replace_value_div',
		),
	);


	if ( $direct_input ) {
		$form['valueset']['value'] = array(
			'#type' => 'textfield',
			'#title' => 'Value',
			'#size' => 30,
			'#default_value' => isset($condition) ? $condition['value'] : NULL,
		);
	}
	else {
		$form['valueset']['value_list'] = array(
			'#type' => 'select',
			'#title' => 'Customer\'s Account Balance',
			'#options' => $counters,
		);
	}

	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save configuration'),
	);

	$form['actions']['cancel'] = array(
		'#type' => 'submit',
		'#value' => t('Cancel'),
	);

	return $form;
}


