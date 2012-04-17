<?php

function ocs_admin_event_update( $form, &$form_state, $local_file = NULL)
{
    $form['ocs'] = array(
        '#type' => 'fieldset',
        '#title' => 'OCS(Online Charging System)',
        '#description' => t('OCS Configuration. Settings can be changed ').
            l( t('here'), '/admin/config/system/ocs_admin'),
        '#collapsible' => TRUE, 
        '#collapsed' => FALSE,
    );

    $ip_address = variable_get('ocs_admin_ip_address', 'Administrative Warning');
    $user_account = variable_get('ocs_admin_user_account', 'Administrative Warning');
    $user_password = variable_get('ocs_admin_user_password', 'Administrative Warning');
    $config_directory = variable_get('ocs_admin_config_directory', 'Administrative Warning');
    $event_file = variable_get('ocs_admin_ocs_event_file', 'Administrative Warning');

    $form['ocs']['ip_address'] = array(
        '#type' => 'item',
        '#title' => 'IP Address',
        '#markup' => $ip_address,
    );

    $form['ocs']['user_account'] = array(
        '#type' => 'item',
        '#title' => 'Account Name',
        '#markup' => $user_account,
    );

    $form['ocs']['config_directory'] = array(
        '#type' => 'item',
        '#title' => 'Configuration Directory',
        '#markup' => $config_directory,
    );

    $form['ocs']['ocs_event_file'] = array(
        '#type' => 'item',
        '#title' => 'Event File Name',
        '#markup' => $event_file,
    );

    // get file from the OCS server by ftp
    $file_content = NULL;
    $error_msg = NULL;
    $ftp = ftp_connect( $ip_address);
    if ( $ftp) {
        $result = ftp_login( $ftp, $user_account, $user_password);
        if ( $result ) {
            ftp_pasv( $ftp, 1);
            ftp_chdir( $ftp, $config_directory);
            if ( ftp_get( $ftp, '/tmp/'. $event_file, $event_file, FTP_ASCII)) {
                $local_file = '/tmp/'.$event_file;
                $file_content = file_get_contents( $local_file);
            }
            else {
                $error_msg = t('Fail to get file %file from the server %addr.',
                    array( '%file' => $config_directory . $event_file, '%addr' => $ip_address));
            }
            ftp_close( $ftp);
        }
        else {
            $error_msg = t('Fail to login to the server %addr with user id "%user".',
                array( '%addr' => $ip_address, '%user' => $user_account));
        }
    }
    else {
        $error_msg = t('Fail to connect the server %addr',
            array( '%addr' => $ip_address));
    }

    if ( isset($error_msg)) {
        drupal_set_message( $error_msg, 'warning');

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => 'Cancel',
        );

    }
    else {
        $form['ocs']['file_content'] = array(
            '#type' => 'textarea',
            '#title' => 'Event File Contents',
            '#default_value' => $file_content,
        );

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => 'Update Configuration'
        );

    }

    return $form;
}


/*
    submit OCS configuration
*/
function ocs_admin_event_update_submit( $form, &$form_state) 
{
    if ( ocs_admin_event_save( $form_state['values']['file_content'])) {
        drupal_set_message( t('System events are updated.'));
    }
    else {
        drupal_set_message( t('Failed to update events.'), 'warning');
    }

    $form_state['redirect'] = 'ocs_admin/event';

}

function ocs_admin_event_save( $event_json)
{
    $json = json_decode( $event_json, true);

    if ( update_ocs_events( $json["OCS_EVENT"]) &&
        update_ocs_states( $json["OCS_STATE"]) )
        return TRUE;

    return FALSE;
}

function update_ocs_events( $events)
{
    // load all events
    $node_nids = array();
    $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_events'));
    foreach( $items as $item) {
        if ( isset( $item->field_ocs_event_id['und'][0]['value'])) {
            // save nid
            $node_nids[ $item->field_ocs_event_id['und'][0]['value'] ] = $item->nid;
        }
        else {
            // invalid node, remove it
            dpm( $item, 'remove item');
            node_delete( $item->nid );
        }
    }

    /* update OCS Events
    // save all event nodes
    // json object format
    // {"OCS_ACTION":[{"actionName":"activatePromotion"},{"actionName":"sendNotification"}],
    // "OCS_EVENT":{
        "OCS_NOT_ENOUGH_BALANCE":10,
        "OCS_OP_CHANGE_STATUS":20,
        "OCS_FIRST_CALL":1,
        "OCS_EXPIRE_STATE":30,
        "OCS_RECHARGE":2
        },
        "OCS_STATE":{"ACTIVE":1,"TERMINATE":4,"NUM_PRESERVE":3,"USER_STOP":6,"IDLE":0,"BLOCK":5,"SUSPEND":2}}
    */

    foreach( $events as $event_name => $event_id) {
        if ( isset( $node_nids[ $event_id])) {
            // client id exist, modify it
            $node = node_load( $node_nids[ $event_id]);
            unset( $node_nids[ $event_id]);
        }
        else {
            // Create a node object, and add node properties.
            $node = _get_new_node( 'ocs_events');
            dpm( $node, 'new node');
        }

        $node->title = $event_name;
        $node->field_ocs_client_id['und'][0]['tid'] = ocs_admin_get_term_id( 'ocs_client_id', 'OCS');
        $node->field_ocs_event_export['und'][0]['value'] = 0;
        $node->field_ocs_event_id['und'][0]['value'] = (string)$event_id;

        node_save( $node);
    }

    /*
        format : 
        'event_id' => 'nid'
    */
    dpm( $node_nids, 'remove nids');
    foreach( $node_nids as $nid) {
        dpm( $nid, 'remove node nid');
        node_delete( $nid);
    }

    drupal_set_message( t( 'OCS Events are updated. Check it ') . l( t('here'), 'ocs_admin/event'));

    return TRUE;
}

function update_ocs_states( $states)
{
    // load all states
    $node_nids = array();
    $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_account_state'));
    foreach( $items as $item) {
        if ( isset( $item->field_ocs_state_id['und'][0]['value'])) {
            // save nid
            $node_nids[ $item->field_ocs_state_id['und'][0]['value'] ] = $item->nid;
        }
        else {
            // invalid node, remove it
            dpm( $item, 'remove item');
            node_delete( $item->nid );
        }
    }

    /*
        Update OCS States

        "OCS_STATE":{
            "ACTIVE":1,
            "TERMINATE":4,
            "NUM_PRESERVE":3,
            "USER_STOP":6,
            "IDLE":0,
            "BLOCK":5,
            "SUSPEND":2}
    */
    foreach( $states as $state_name => $state_id) {
        if ( isset( $node_nids[ $state_id])) {
            // client id exist, modify it
            $node = node_load( $node_nids[ $state_id]);
            unset( $node_nids[ $state_id]);
        }
        else {
            // Create a node object, and add node properties.
            $node = _get_new_node( 'ocs_account_state');
            dpm( $node, 'new node');
        }

        $node->title = $state_name;
        $node->field_ocs_state_id['und'][0]['value'] = (string)$state_id;

        node_save( $node);
    }

    drupal_set_message( t( 'OCS States are updated. Check it ') . l( t('here'), 'ocs_admin/state'));

    return TRUE;
}

function _get_new_node( $type)
{
    $node = (object) NULL;
    $node->type = $type;
    $node->uid = 0;
    $node->created = strtotime("now");
    $node->changed = strtotime("now");
    $node->status = 1;
    $node->comment = 0;
    $node->promote = 0;
    $node->moderate = 0;
    $node->sticky = 0;
    $node->language = 'und';

    node_object_prepare($node);

    return $node;
}

function ocs_admin_get_term_id( $vocabulary_name, $term)
{
    $voca = &drupal_static( __FUNCTION__, array());
//    dpm( $voca, 'voca');

    // get vocabulary
    if ( ! isset( $voca[ $vocabulary_name])) {
        $v = taxonomy_vocabulary_machine_name_load($vocabulary_name);
        $voca[ $vocabulary_name] = array(
            'vid' => $v->vid,
        );
    }
    $vid = $voca[ $vocabulary_name]['vid'];

    if ( ! isset($voca[$vocabulary_name][$term])) {
        // get term id from vocabulary

        $query = new EntityFieldQuery;
        $result = $query
            ->entityCondition('entity_type', 'taxonomy_term')
            ->propertyCondition('name', $term)
            ->propertyCondition('vid', $vid)
            ->execute();

//        dpm( $result, 'voca query');

        foreach( $result['taxonomy_term'] as $key => $value) {
            $voca[ $vocabulary_name][ $term] = $value->tid;
            break;
        }
    }
    return $voca[ $vocabulary_name][ $term];
}
