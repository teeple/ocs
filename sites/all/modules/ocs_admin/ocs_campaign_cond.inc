<?php

function ocs_admin_campaign_add_cond_form( $form, &$form_state, $campaign_id = NULL, $cond_id=NULL)
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

	if ( isset( $cond_id)) {
		// if cond_id is not NULL, it's editing mode
		$condition = ocs_admin_campaign_cond_load( $cond_id);
//		dpm( $condition, 'condition');
		$cond = $condition['event_id'].':'.$condition['param_id'];
	}

//	dpm( $options, 'options');
	$form = array();

	$form['#campaign_id'] = $campaign_id;
	$form['#paramlist'] = $paramlist;
	$form['#cond_id'] = $cond_id;

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

function _campaign_operator( $paramlist, $selected)
{
//	dpm( $paramlist, 'param');
	$oplist = array(
		OCS_PARAM_TYPE_STRING => array(
			'string', 'contains', 'equal to', 'not equal to'
			),
		OCS_PARAM_TYPE_INTEGER => array(
			'integer', 'equal to', 'not equal to', 'greater than', 'less than'
			),
		OCS_PARAM_TYPE_ENUM => array(
			'enum', 'contains', 'equal to', 'not equal to'
			),
		OCS_PARAM_TYPE_ACCOUNT_KEY => array(
			'account key', 'contains', 'equal to', 'not equal to'
			),
		OCS_PARAM_TYPE_GROUP_ID => array(
			'group id', 'contains', 'equal to', 'not equal to'
			),
	);

//	dpm( $selected, 'selected op');
	list( $event, $param) = explode( ':', $selected);
	if ( isset( $paramlist[ $param])) {
		$para_type = $paramlist[ $param];
//		dpm( $paramlist, 'paramlist');
		return drupal_map_assoc( $oplist[$para_type]);
	}

	return drupal_map_assoc( array('unknown parameter type'));
}

function ocs_admin_ajax_condition_operator( $form, $form_state)
{
	return $form['container'];
}

function ocs_admin_ajax_value_type( $form, $form_state)
{
	return $form['valueset'];
}



function ocs_admin_campaign_add_cond_form_submit( $form, &$form_state)
{
	$values = $form_state['values'];
	dpm( $values, 'values submit');
	if ( $values['op'] != 'Cancel') {
		list( $event_id, $param_id) = explode( ':', $values['cond']);
		$cond = array(
			'cond_id' => $form['#cond_id'],
			'campaign_id' => $form['#campaign_id'],
			'event_id' => $event_id,
			'param_id' => $param_id,
			'param_name' => $form['#paramlist'][ $values['cond']],
			'operator' => $values['operator'],
			'value' => $values['value'],
			'cond_name' => t( '@para @op @value', array( 
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
function ocs_admin_campaign_condition_save( $cond)
{
    db_set_active('ocsdb');

	$result = db_merge( 'ocs_campaign_conditions')
		->key( array( 'cond_id' => $cond['cond_id']))
		->fields( $cond)
		->execute();
    dpm( $result, 'db_merge result');

    db_set_active( 'default');

	return $result;
}


function ocs_admin_campaign_cond_delete_confirm( $form, &$form_state, $campaign_id, $cond_id)
{
	$cond_name = isset( $_GET['name']) ? $_GET['name'] : 'Condition ID:' . $cond_id;

    $form['#campaign_id'] = $campaign_id;
    $form['#cond_id'] = $cond_id;
    $form['#cond_name'] = $cond_name;
    $caption = '<p>' . 
            t('<strong>Warning:</strong> The condition "%key" will be deleted from the Campaign.',
                array( '%key' => $cond_name)) .
            '</p>';

    $caption .= '<p>'. t('This action cannot be undone.') . '</p>';

    return confirm_form( $form, t('Are you sure to delete the event "%key"?',
        array( '%key' => $cond_name)),
        'ocs_admin/campaign/' . $campaign_id . '/rule',
        $caption, t('Delete'));
}

function ocs_admin_campaign_cond_delete_confirm_submit( $form, &$form_state)
{
    $campaign_id = $form['#campaign_id'];
    $cond_id = $form['#cond_id'];
    $cond_name = $form['#cond_name'];
    ocs_admin_campaign_condition_delete( $cond_id);

    drupal_set_message( t('The condition "%key" has been removed from the Campaign.', 
			array( '%key' => $cond_name)));

    $form_state['redirect'] = 'ocs_admin/campaign/' . $campaign_id . '/rule';
}

function ocs_admin_campaign_condition_delete( $cond_id)
{
    db_set_active( 'ocsdb');

	// delete event from campaign
	$result = db_delete( 'ocs_campaign_conditions')
		->condition( 'cond_id', $cond_id, '=')
		->execute();
	dpm( $result, 'ocs_campaign_condition result');

    db_set_active( 'default');
}

function ocs_admin_campaign_cond_load( $cond_id)
{
    db_set_active( 'ocsdb');

    $query = db_select( 'ocs_campaign_conditions', 'c');
    $result = $query->condition( 'c.cond_id', $cond_id, '=')
        ->fields( 'c')
        ->execute()
        ->fetchAssoc();
 
    db_set_active( 'default');

	return $result;
}


/*
   add or delete conditions

   when adding AND or OR condition : AND/add, OR/add
   when deleting condition: <id>/delete
   */
function ocs_admin_campaign_condition_add( $campaign_id, $js, $cond_id, $op)
{
	if ( $op == 'delete') {
		ocs_admin_campaign_condition_delete( $cond_id);
	}
	else {	// add
		// add AND condition
		$cond = array(
			'cond_id' => NULL,
			'campaign_id' => $campaign_id,
			'event_id' => 0,
			'param_id' => 0,
			'param_name' => 'AND',
			'operator' => 'AND',
			'value' => NULL,
			'cond_name' => strtoupper($cond_id),
		);
		ocs_admin_campaign_condition_save( $cond);
	}

	$form = drupal_get_form( 'ocs_admin_campaign_rule_form', $campaign_id);
    $header = array(
        t('Name'),
        t('Weight'),
        array( 'data' => t('Operations'), 'colspan' => '3'),
    );

    // event table
    $output = '<div id="ocs_admin_campaign_cond_div">'. 
		ocs_admin_tree_table_theme( $form, $header) . '</div>' ;

	if ( $js) {
		ctools_include('ajax');
		$commands = array();
		$commands[] = ajax_command_replace( '#ocs_admin_campaign_cond_div', $output);
		print ajax_render( $commands);
		exit;
	}
	else {
		return $output;
	}
}

/*
    get condition list
*/
function ocs_admin_campaign_conditions_load_all( $campaign_id)
{
    db_set_active('ocsdb');

    // get group information
    $query = db_select( 'ocs_campaign_conditions', 'c');
    $result = $query->condition( 'c.campaign_id', $campaign_id, '=')
        ->fields( 'c')
        ->execute()
        ->fetchAllAssoc( 'cond_id', PDO::FETCH_ASSOC);

//    dpm( $result, 'result');

    db_set_active( 'default');

    return $result;
}
