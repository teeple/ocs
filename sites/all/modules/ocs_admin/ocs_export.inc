<?php

/*
    state transition rule form
*/
function ocs_admin_export_state_transition( $form, &$form_state)
{
    $form['desc'] = array(
        '#type' => 'markup',
        '#markup' => '<p>Check the Json file contents that contains the configuration and Press <em>Update Configuration</em> button to upload the file to OCS.</p>',
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
    $file_name = variable_get('ocs_admin_ocs_state_file', 'Administrative Warning');

    $form['ocs']['ip_address_markup'] = array(
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

    $form['ocs']['file_name_markup'] = array(
        '#type' => 'item',
        '#title' => 'Export File Name',
        '#markup' => $file_name,
    );

    $form['ocs']['file_name'] = array(
        '#type' => 'hidden',
        '#value' => $file_name,
    );

    // get view and build json output
    $view = views_get_view( 'ocs_state_machine');
    $view->set_display( 'block');
    $view->execute();
    $result = $view->result;
 //   dpm( $result);

    // build json
    $json = array();
    foreach( $result as $item) {
        $action_list = array();
        foreach( $item->field_field_ocs_action_ref as $action) {
            $action_list[] = $action['rendered']['#markup'];
        }

        foreach( $item->field_field_event as $event) {
            $json[] = array( 'node' => array(
                "Current State" => $item->field_field_from_state[0]['rendered']['#markup'],
                "Next State" => $item->field_field_to_state[0]['rendered']['#markup'],
                "Event" => $event['rendered']['#markup'],
                "Action" => $action_list,
            ));
        }

    }

    $json_file = json_encode( array( 'nodes' => $json));
//    dpm( $json_file, 'json');

    $form['file_content'] = array(
        '#type' => 'textarea',
        '#title' => 'Json File Contents',
        '#default_value' => $json_file,
    );

    $form['actions'] = array('#type' => 'actions');

    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Update Configuration'),
    );

    $form['actions']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
    );

    return $form;
}


function ocs_admin_export_state_transition_submit( $form, &$form_state)
{
    $values = $form_state['values'];

    if ( $values['op'] == 'Update Configuration') {
        $json = $values['file_content'];
        dpm( $json, 'json');

        // write to file
        try{
            $fd = fopen( '/tmp/'. $values['file_name'], 'w');
            fwrite( $fd, $json);
            fclose( $fd);

            $ip_address = variable_get('ocs_admin_ip_address', 'Administrative Warning');
            $user = variable_get('ocs_admin_user_account', 'Administrative Warning');
            $passwd = variable_get('ocs_admin_user_password', 'Administrative Warning');
            $dir = variable_get('ocs_admin_config_directory', 'Administrative Warning');
            $file_name = $values['file_name'];

            // ftp to host
            if ( _ftp_put( $ip_address, $user, $passwd, $dir, $file_name)) {
                drupal_set_message( t('State Configuration file is updated in the OCS'));
            }
        }
        catch( Exception $e) {
            drupal_set_message( $e->getMessage(), 'warning');       
        }
    }

    $form_state['redirect'] = 'ocs_admin/config/ocs/state/transition';
}

function _ftp_put( $ip_address, $user, $passwd, $dir, $file)
{
    $error_msg = NULL;

    // put file to the OCS server by ftp

    $ftp = ftp_connect( $ip_address);
    if ( $ftp) {
        $result = ftp_login( $ftp, $user, $passwd);
        if ( $result ) {
            if ( ftp_chdir( $ftp, $dir)) {
                if ( ! ftp_put( $ftp, $file, '/tmp/'.$file, FTP_ASCII)) {
                    $error_msg = t('Fail to put file %file to the server %addr at %dir.',
                        array( '%file' => $file, '%addr' => $ip_address, '%dir' => $dir));
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


    return TRUE;
}

/*
    export service permission in each state 
*/
function ocs_admin_export_state_svc_perm( $form, &$form_state)
{
    $form['desc'] = array(
        '#type' => 'markup',
        '#markup' => '<p>Check the Json file contents that contains the configuration and Press <em>Update Configuration</em> button to upload the file to OCS.</p>',
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
    $file_name = variable_get('ocs_admin_ocs_svc_perm_file', 'Administrative Warning');

    $form['ocs']['ip_address_markup'] = array(
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

    $form['ocs']['file_name_markup'] = array(
        '#type' => 'item',
        '#title' => 'Export File Name',
        '#markup' => $file_name,
    );

    $form['ocs']['file_name'] = array(
        '#type' => 'hidden',
        '#value' => $file_name,
    );

    // get view and build json output
    $view = views_get_view( 'ocs_state');
    $view->set_display( 'json');
    $view->execute();
    $result = $view->result;

    // build json
    $items = entity_load( 'node', FALSE, array( 'type' => 'ocs_svc_perm'));
    $perm_list = array();
    foreach( $items as $item) {
        $perm_list[ $item->field_ocs_svc_group['und'][0]['value']][$item->title] = false;
    }

    $perm_status = array();
    foreach( $result as $item) {
        $state_name = $item->node_title;
        if ( ! isset( $perm_status[ $state_name])) {
            $perm_status[ $state_name] = $perm_list;
        }
        if ( isset( $item->field_field_ocs_svc_group[0])) {
            $group = $item->field_field_ocs_svc_group[0]['rendered']['#markup'];
            $perm = $item->node_field_data_field_ocs_svc_perm_title;
            $perm_status[ $state_name][$group][$perm] = true;
        }
    }

    // build json
    $json = array();
    foreach( $perm_status as $key => $value) {
        $json[] = array(
            'node' => array_merge( array( 'State' => $key), $value),
        );
    }

    $json_file = json_encode( array( 'nodes' => $json));
//    dpm( $json_file, 'json');

    $form['file_content'] = array(
        '#type' => 'textarea',
        '#title' => 'Json File Contents',
        '#default_value' => $json_file,
    );

    $form['actions'] = array('#type' => 'actions');

    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Update Configuration'),
    );

    $form['actions']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
    );

    return $form;
}


function ocs_admin_export_state_svc_perm_submit( $form, &$form_state)
{
    $values = $form_state['values'];

    if ( $values['op'] == 'Update Configuration') {
        $json = $values['file_content'];
        dpm( $json, 'json');

        // write to file
        try{
            $fd = fopen( '/tmp/'. $values['file_name'], 'w');
            fwrite( $fd, $json);
            fclose( $fd);

            $ip_address = variable_get('ocs_admin_ip_address', 'Administrative Warning');
            $user = variable_get('ocs_admin_user_account', 'Administrative Warning');
            $passwd = variable_get('ocs_admin_user_password', 'Administrative Warning');
            $dir = variable_get('ocs_admin_config_directory', 'Administrative Warning');
            $file_name = $values['file_name'];

            // ftp to host
            if ( _ftp_put( $ip_address, $user, $passwd, $dir, $file_name)) {
                drupal_set_message( t('State Configuration file is updated in the OCS'));
            }
        }
        catch( Exception $e) {
            drupal_set_message( $e->getMessage(), 'warning');       
        }
    }

    $form_state['redirect'] = 'ocs_admin/config/ocs/svcperm';
}


