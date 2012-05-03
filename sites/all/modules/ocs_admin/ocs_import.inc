<?php

function ocs_admin_import_title( $arg)
{
    return 'Import Configuration from '.$arg;
}


function ocs_admin_import_ocs_config( $form, &$form_state, $local_file = NULL)
{
    $form['desc'] = array(
        '#type' => 'markup',
        '#markup' => '<p>Import configuration file from OCS.</p>',
    );

    $form['ocs'] = array(
        '#type' => 'fieldset',
        '#title' => 'OCS(Online Charging System) Access Info',
        '#description' => t('OCS Access Settings. It can be changed ').
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

    $form['actions'] = array('#type' => 'actions');

    $file_content = _ftp_get( $ip_address, $user_account, $user_password, $config_directory, $event_file);
    if ( $file_content) {
        $form['file_content'] = array(
            '#type' => 'textarea',
            '#title' => 'Event File Contents',
            '#default_value' => $file_content,
        );

        $form['actions']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Update Configuration'),
        );

    }

    $form['actions']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
    );


    return $form;
}


function _ftp_get( $ip_address, $user, $passwd, $dir, $file)
{
    // get file from the OCS server by ftp
    $file_content = NULL;
    $error_msg = NULL;
    $ftp = ftp_connect( $ip_address);
    if ( $ftp) {
        $result = ftp_login( $ftp, $user, $passwd);
        if ( $result ) {
            ftp_pasv( $ftp, 1);
            if ( ftp_chdir( $ftp, $dir)) {
                if ( ftp_get( $ftp, '/tmp/'. $file, $file, FTP_ASCII)) {
                    $local_file = '/tmp/'.$file;
                    $file_content = file_get_contents( $local_file);
                }
                else {
                    $error_msg = t('Fail to get file %file from the server %addr.',
                        array( '%file' => $dir . $file, '%addr' => $ip_address));
                }
            }
            else {
                $error_msg = t('Fail to change directory to %dir', array( '%dir' => $dir));
            }
            ftp_close( $ftp);
        }
        else {
            $error_msg = t('Fail to login to the server %addr with user id "%user".',
                array( '%addr' => $ip_address, '%user' => $user));
        }
    }
    else {
        $error_msg = t('Fail to connect the server %addr',
            array( '%addr' => $ip_address));
    }

    if ( $error_msg) {
        drupal_set_message( $error_msg, 'warning');
        return FALSE;
    }

    return $file_content;
}

/*
    submit OCS configuration
*/
function ocs_admin_import_ocs_config_submit( $form, &$form_state)
{
    if ( $form_state['values']['op'] != 'Cancel') {
        if ( ocs_admin_event_save( $form_state['values']['file_content'])) {
            drupal_set_message( t('System events are updated.'));
        }
        else {
            drupal_set_message( t('Failed to update events.'), 'warning');
        }
    }

    $form_state['redirect'] = 'ocs_admin/config/ocs';

}

function ocs_admin_event_save( $event_json)
{
    $json = json_decode( $event_json, true);

    if ( update_ocs_param( $json["OCS_PARAM"]) &&
        update_ocs_events( $json["OCS_EVENT"]) &&
        update_ocs_states( $json["OCS_STATE"]) &&
        update_ocs_actions( $json["OCS_ACTION"]) )
        return TRUE;

    return FALSE;
}

function update_ocs_param( $param)
{
    // load all events
    $node_nids = array();
    $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_param'));
    foreach( $items as $item) {
        // save nid
        $node_nids[ $item->title ] = $item->nid;
    }

    /* update OCS Events
    // save all event nodes
    // json object format

    /*
    "OCS_PARAM":
        {"CALLING":
            {"param_desc":"",
            "param_type":"END_USER_E164"},
        "REMAINS":
            {"param_desc":"",
            "param_type":"ACCOUNT"},
        "CALLED":
            {"param_desc":"",
            "param_type":"END_USER_IMSI"},
        "CALL_TYPE":
            {"param_desc":"",
            "param_type":"PC_TYPE"}},

    */

    foreach( $param as $param_name => $param_info) {
        if ( isset( $node_nids[ $param_name])) {
            // client id exist, modify it
            $node = node_load( $node_nids[ $param_name]);
            unset( $node_nids[ $param_name]);
        }
        else {
            // Create a node object, and add node properties.
            $node = _get_new_node( 'ocs_param');
            dpm( $node, 'new node');
        }

        $node->title = $param_name;
        $node->field_ocs_client_id['und'][0]['tid'] = ocs_admin_get_term_id( 'ocs_client_id', 'OCS');
        $node->field_description['und'][0]['value'] = $param_info['param_desc'];
        $node->field_ocs_param_type['und'][0]['value'] = ocs_admin_get_param_type( $param_info['param_type']);
        $node->field_ocs_param_value['und'][0]['value'] = isset( $param_info['param_value']) ? json_encode( $param_info['param_value']) : '';

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

    drupal_set_message( t( 'OCS Params are updated. Check it ') . l( t('here'), 'ocs_admin/param'));

    return TRUE;
}

function update_ocs_events( $events)
{
    // load all events
    $node_nids = array();
    $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_events'));
    foreach( $items as $item) {
        // save nid
        $node_nids[ $item->title ] = $item->nid;
    }

    /* update OCS Events
    // save all event nodes
    // json object format

/*
"OCS_EVENT":
    {"OCS_RECHARGE":
        {"desc":"Account status has changed",
        "param":["OCS_PARAM.CALLING","OCS_PARAM.CALLED","OCS_PARAM.CALL_TYPE"],
        "id":2},
    ...
    */

    foreach( $events as $event_name => $event_info) {
        if ( isset( $node_nids[ $event_name])) {
            // client id exist, modify it
            $node = node_load( $node_nids[ $event_name]);
            unset( $node_nids[ $event_name]);
        }
        else {
            // Create a node object, and add node properties.
            $node = _get_new_node( 'ocs_events');
            dpm( $node, 'new node');
        }

        $node->title = $event_name;
        $node->field_ocs_client_id['und'][0]['tid'] = ocs_admin_get_term_id( 'ocs_client_id', 'OCS');
        $node->field_ocs_event_export['und'][0]['value'] = 0;
        $node->field_ocs_event_id['und'][0]['value'] = $event_info['id'];
        $node->field_description['und'][0]['value'] = $event_info['desc'];

        unset( $node->field_ocs_param_ref['und']);
        foreach( $event_info['param'] as $name) {
            $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_param', 'title' => $name));

            foreach ( $items as $item) {
                $node->field_ocs_param_ref['und'][] = array( 'target_id' => $item->nid);
            }
        }

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
        // save nid
        $node_nids[ $item->title ] = $item->nid;
    }

    /*
        Update OCS States

    "OCS_STATE":{"ACTIVE":{"desc":"ACTIVE state","id":1},"TERMINATE":{"desc":"TERMINATE state","id":4},"NUM_PRESERVE":{"desc":"NUM_PRESERVE state","id":3},"USER_STOP":{"desc":"USER_STOP state","id":6},"IDLE":{"desc":"IDLE state","id":0},"BLOCK":{"desc":"BLOCK state","id":5},"SUSPEND":{"desc":"SUSPEND state","id":2}}}

*/

    foreach( $states as $state_name => $state_info) {
        if ( isset( $node_nids[ $state_name])) {
            // client id exist, modify it
            $node = node_load( $node_nids[ $state_name]);
            unset( $node_nids[ $state_name]);
        }
        else {
            // Create a node object, and add node properties.
            $node = _get_new_node( 'ocs_account_state');
            dpm( $node, 'new node');
        }

        $node->title = $state_name;
        $node->field_ocs_state_id['und'][0]['value'] = (string)$state_info['id'];

        node_save( $node);
    }

    drupal_set_message( t( 'OCS States are updated. Check it ') . l( t('here'), 'ocs_admin/state'));

    return TRUE;
}

function update_ocs_actions( $actions)
{
    // load all events
    $node_nids = array();
    $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_actions'));
    foreach( $items as $item) {
        // save nid
        $node_nids[ $item->title ] = $item->nid;
    }

    /* update OCS Actions
    // save all event nodes
    // json object format

    /*
    {"OCS_ACTION":
        {"sendNotification":
            {"desc":"send SMS notification",
            "param":["OCS_PARAM.CALLING","OCS_PARAM.CALLED","OCS_PARAM.CALL_TYPE"]
            },
        ...
            
        */
 
    foreach( $actions as $action_name => $action_info) {
        if ( isset( $node_nids[ $action_name])) {
            // client id exist, modify it
            $node = node_load( $node_nids[ $action_name]);
            unset( $node_nids[ $action_name]);
        }
        else {
            // Create a node object, and add node properties.
            $node = _get_new_node( 'ocs_actions');
            dpm( $node, 'new node');
        }

        $node->title = $action_name;
        $node->field_ocs_client_id['und'][0]['tid'] = ocs_admin_get_term_id( 'ocs_client_id', 'OCS');
        $node->field_description['und'][0]['value'] = $action_info['desc'];

        unset( $node->field_ocs_param_ref['und']);
        foreach( $action_info['param'] as $name) {
            $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_param', 'title' => $name));
            foreach ( $items as $item) {
                $node->field_ocs_param_ref['und'][] = array( 'target_id' => $item->nid);
            }
        }


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
    if ( ! isset( $voca[ $vocabulary_name])) {
        $voca = ocs_admin_get_all_terms( $vocabulary_name);
        dpm( $voca, 'get all voca terms');
    }
    return $voca[ $vocabulary_name][$term] -> tid;
}

function ocs_admin_get_all_terms( $vocabulary_name)
{
    $voca = &drupal_static( __FUNCTION__, array());
    if ( isset( $voca[ $vocabulary_name])) return $voca;

    // get vocabulary
    $vid = taxonomy_vocabulary_machine_name_load($vocabulary_name);

    // get term id from vocabulary

    $query = new EntityFieldQuery;
    $result = $query
        ->entityCondition('entity_type', 'taxonomy_term')
        ->propertyCondition('vid', $vid->vid)
        ->execute();

    //dpm( $result, 'voca query');
    $terms = taxonomy_term_load_multiple( array_keys($result['taxonomy_term']));

    foreach( $terms as $term) {
        $voca[ $vocabulary_name][ $term->name ] = $term;
    }

    return $voca;
}


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

    $form_state['redirect'] = 'ocs_admin/config/ocs/event';
}
