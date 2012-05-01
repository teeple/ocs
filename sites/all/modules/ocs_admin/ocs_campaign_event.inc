<?php

function ocs_admin_campaign_event_form( $form, &$form_state, $campaign_id)
{
    $form['body'] = array(
        '#type' => 'markup',
        '#markup' => t( 'Select the event to add.'),
    );

    $form['campaign_id'] = array(
        '#type' => 'hidden',
        '#value' => $campaign_id,
    );

    $items = entity_load('node', FALSE, array('type' => 'ocs_events'));
    $options = array();
    foreach( $items as $item) {
        $client = taxonomy_term_load( $item->field_ocs_client_id['und'][0]['tid']);
//        dpm( $client, 'client');
        $options[$client->name][$item->nid] = $item->title;
    }

	$form['event_id'] = array(
		'#type' => 'select',
		'#title' => t('React on event'),
		'#options' => $options,
		'#description' => t('Whenever the event occurs, rule evaluation is trigged'),
	);

	/*
  	$event_status = ocs_admin_campaign_events_load_all( $campaign_id);
	dpm( $event_status, 'event');

	foreach( $options as $client => $events) {
		$form[$client] = array(
			'#type' => 'fieldset',
			'#title' => $client . ' Events',
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
		);

		$form[$client][$client.'event_id'] = array(
			'#type' => 'checkboxes',
			'#title' => t('React on event'),
			'#options' => $events,
			'#default_value' => array_keys( $event_status),
			'#description' => t('Whenever the event occurs, rule evaluation is trigged'),
		);
	}
	*/

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save configuration'),
    );

    return $form;
}

function ocs_admin_campaign_event_form_submit( $form, &$form_state)
{
    dpm( $form_state['values'], 'form_state');
    $event = $form_state['values'];

    ocs_admin_campaign_event_save( $event);

    $form_state['redirect'] = 'ocs_admin/campaign/' . $event['campaign_id'] .'/rule';
}

/*
    get condition list
*/
function ocs_admin_campaign_events_load_all( $campaign_id)
{
    db_set_active('ocsdb');

    // get group information
    $query = db_select( 'ocs_campaign_events', 'c');
    $result = $query->condition( 'c.campaign_id', $campaign_id, '=')
        ->fields( 'c')
        ->execute()
        ->fetchAllAssoc( 'event_id', PDO::FETCH_ASSOC);
    

    db_set_active( 'default');

    return $result;
}

/*
** delete an event
*/
function ocs_admin_campaign_event_delete_confirm( $form, &$form_state, $campaign_id, $event_id)
{
	$event_name = isset( $_GET['event_name']) ? $_GET['event_name'] : $event_id;

    $form['#campaign_id'] = $campaign_id;
    $form['#event_id'] = $event_id;
    $form['#event_name'] = $event_name;
    $caption = '<p>' . 
            t('<strong>Warning:</strong> The event "%key" will be deleted from the Campaign.',
                array( '%key' => $event_name)) .
            '</p>';

    $caption .= '<p>'. t('This action cannot be undone.') . '</p>';

    return confirm_form( $form, t('Are you sure to delete the event "%key"?',
        array( '%key' => $event_name)),
        'ocs_admin/campaign/' . $campaign_id . '/rule',
        $caption, t('Delete'));
}

function ocs_admin_campaign_event_delete_confirm_submit( $form, &$form_state)
{
    $campaign_id = $form['#campaign_id'];
    $event_id = $form['#event_id'];
    $event_name = $form['#event_name'];
    ocs_admin_campaign_delete_event( $campaign_id, $event_id);

    drupal_set_message( t('The event %key has been removed from the Campaign.', 
			array( '%key' => $event_name)));

    $form_state['redirect'] = 'ocs_admin/campaign/' . $campaign_id . '/rule';
}

function ocs_admin_campaign_delete_event( $campaign_id, $event_id)
{
    db_set_active( 'ocsdb');

	// delete event from campaign
	$result = db_delete( 'ocs_campaign_events')
		->condition( 'campaign_id', $campaign_id, '=')
		->condition( 'event_id', $event_id, '=')
		->execute();
	dpm( $result, 'ocs_campaign_event result');

    db_set_active( 'default');
}

/*
    save OCS campaign event
*/
function ocs_admin_campaign_event_save( $campaign)
{
    db_set_active( 'ocsdb');

    $fields = array();
	foreach( array( 'campaign_id', 'event_id') as $key) {
		$fields[$key] = $campaign[$key];
	}

    $status = db_merge( 'ocs_campaign_events')
        ->key( array( 'event_id' => $campaign['event_id'], 'campaign_id' => $campaign['campaign_id']))
        ->fields( $fields)
        ->execute();
    dpm( $status, 'db_merge result');

    db_set_active( 'default');
}
 

