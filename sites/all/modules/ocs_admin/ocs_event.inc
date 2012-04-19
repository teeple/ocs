<?php

/*
    edit event node
*/
function ocs_admin_event_export( $form, &$form_state, $event_name)
{
    // load event node
    $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_events', 'title' => $event_name));
    $event = array_shift( $items);
//    dpm( $event, 'event');

    $form['#node'] = $event;

    $form['title'] = array(
        '#type' => 'item',
        '#title' => 'Event Name',
        '#markup' => $event_name,
    );

    $form['export'] = array(
        '#type' => 'radios',
        '#title' => 'Export Status',
        '#description' => 'Export Status of this Event',
        '#options' => array(
            0 => t('Off'),
            1 => t('On'),
        ),
        '#default_value' => $event->field_ocs_event_export['und'][0]['value'],
    );

    // load client IDs
    $clients = ocs_admin_get_all_terms( 'ocs_client_id');
//    $options = drupal_map_assoc( array_keys($clients['ocs_client_id']));

    // build term index
    $options = array();
    foreach( $clients['ocs_client_id'] as $name => $value) {
        $options[ $value->tid] = $name;
    }
//    dpm( $options, 'options');
    
   // check the current value
    $default_value = array();
    if ( count($event->field_ocs_subscribed_client) > 0) {
        foreach( $event->field_ocs_subscribed_client['und'] as $item) {
            $default_value[] = $item['tid'];
        }
    }
//    dpm( $default_value, 'default_value');

    // unset this client id from options
    $my_client_id = $event->field_ocs_client_id['und'][0]['tid'];
    unset( $options[ $my_client_id]);

 
    $form['subscription'] = array(
        '#type' => 'checkboxes',
        '#title' => 'Subscribe Event',
        '#description' => 'Subscription Status of this Event in each Client',
        '#options' => $options,
        '#default_value' => $default_value,
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

    $form['actions']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
    );

    return $form;
}


/*
    submit OCS configuration
*/
function ocs_admin_event_export_submit( $form, &$form_state) 
{
//    dpm( $form_state['values'], 'value');
    $values = $form_state['values'];

    if ( $values['op'] == 'Save') {
        $node = $form['#node'];
 //       dpm( $node, 'node');

        // set export status
        $node->field_ocs_event_export['und'][0]['value'] = $values['export'];

        // reset subscribed client list
        unset( $node->field_ocs_subscribed_client);
        foreach( $values['subscription'] as $tid => $value) {
            if ( $value) {
                $node->field_ocs_subscribed_client['und'][] = array( 'tid' => $tid);
            }
        }

        node_save( $node);
    }

    $form_state['redirect'] = 'ocs_admin/event';
}
