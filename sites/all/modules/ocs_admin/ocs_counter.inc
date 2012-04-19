<?php

function ocs_admin_counter_edit_form( $form, &$form_state, $account_key, $subscription_key, $counter_id)
{
    $form['account_key_markup'] = array(
        '#type' => 'item',
        '#title' => 'Account Key',
        '#markup' => $account_key,
    );

    $form['account_key'] = array(
        '#type' => 'hidden',
        '#value' => $account_key,
    );

    $form['subscription_key'] = array(
        '#type' => 'hidden',
        '#value' => $subscription_key,
    );

    $form['counter_id'] = array(
        '#type' => 'hidden',
        '#value' => $counter_id,
    );

    $counter = ocs_admin_counter_load( $subscription_key, $counter_id);
    $form['#counter'] = $counter;
//    dpm( $counter, 'counter');

    // load counter name info
    $counter_name = ocs_admin_counter_name_load( $counter_id);
//    dpm( $counter_name, 'counter name');

    $form['counter_scale'] = array(
        '#type' => 'hidden',
        '#value' => $counter_name['counter_scale'],
    );

    $form['value'] = array(
        '#type' => 'textfield',
        '#title' => 'Value',
        '#description' => 'Enter the new value for this counter',
        '#default_value' => ($counter_name['counter_scale'] > 0) ? 
            $counter['counter_value'] / $counter_name['counter_scale'] : $counter['counter_value'],
    );

    $date_str = date( 'Y-m-d', $counter['counter_expire']/1000);
//    dpm( $date_str, 'date str');
    $form['expire'] = array(
        '#type' => 'date',
        '#title' => 'Expire Date',
        '#description' => 'Enter the new expiration date',
        '#default_value' => date_parse( $date_str),
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
function ocs_admin_counter_edit_form_submit( $form, &$form_state) 
{
    if ( $form_state['values']['op'] == 'Save') {
        $counter = $form['#counter'];
        $counter_scale = $form_state['values']['counter_scale'];

        $counter['counter_value'] = $form_state['values']['value'];
        if ($counter_scale > 0 ) {
            $counter['counter_value'] *= $counter_scale;
        }
        
        $date = $form_state['values']['expire'];
        $counter['counter_expire'] = strtotime( sprintf( '%04d-%02d-%02d', $date['year'] , $date['month'] , $date['day'])) * 1000;
        dpm( $date, 'date');
        dpm( $counter, 'save counter');

        ocs_admin_counter_save( $counter);
    }
    
    $form_state['redirect'] = 'ocs_admin/account/all/' . $form_state['values']['account_key'];
}

/*
load counter
*/
function ocs_admin_counter_load( $subscription_key, $counter_id)
{
    db_set_active('ocsdb');

    $query = db_select( 'abmf_counter', 'a');
    $result = $query->fields( 'a')
                    ->condition( 'subscription_key', $subscription_key, '=')
                    ->condition( 'counter_id', $counter_id, '=')
                    ->execute()
                    ->fetchAssoc();
   
    db_set_active('default');

    return $result;
}

/*
save counter
*/
function ocs_admin_counter_save( $counter)
{
    db_set_active('ocsdb');

    $result = db_merge( 'abmf_counter')
        ->key( array( 
            'subscription_key' => $counter['subscription_key'],
            'counter_id' => $counter['counter_id'],
            ))
        ->fields( $counter)
        ->execute();
    dpm( $result, 'db_update abmf_counter result');

    db_set_active('default');
    
    return $result;
}

function ocs_admin_counter_name_load( $counter_id) 
{
    $all_counter = ocs_admin_counter_name_load_all();
//    dpm( $all_counter, 'all counter');

    return isset( $all_counter[$counter_id]) ? $all_counter[$counter_id] : FALSE;
}

/*
    load all group
*/
function ocs_admin_counter_name_load_all()
{
   $counter = &drupal_static(__FUNCTION__);
   if ( !isset( $counter)) {
       // TODO this cache does not work. check it later..
       if ( $cache = cache_get('ocs_admin_counter_name')) {
           $counter = $cache->data;
       }
       else {
           db_set_active('ocsdb');

           $query = db_select( 'countername_tbl', 'g');
           $counter = $query->fields( 'g')
                ->execute()
                ->fetchAllAssoc( 'counter_id', PDO::FETCH_ASSOC);

            dpm( $counter, 'fetched counter name');
            cache_set( 'ocs_admin_counter_name', $counter);
//            dpm( cache_get( 'ocs_admin_counter_name'), 'cache after setting');

            db_set_active('default');
       }
    } 

    return $counter;
}

function ocs_admin_counter_add_form( $form, &$form_state, $account_key)
{
    $form['account_key_markup'] = array(
        '#type' => 'item',
        '#title' => 'Account Key',
        '#markup' => $account_key,
    );

    $form['account_key'] = array(
        '#type' => 'hidden',
        '#value' => $account_key,
    );

    $account = ocs_admin_account_load( $account_key);

    $form['subscription_key'] = array(
        '#type' => 'hidden',
        '#value' => $account['subscription_key'],
    );

    $all_counter = ocs_admin_counter_name_load_all();
//    dpm( $all_counter, 'all counter');
    $form['#all_counter'] = $all_counter;

    $options = array();
    foreach( $all_counter as $counter) {
        $options[ $counter['counter_id']] = $counter['counter_name'];
    }

    $form['type'] = array(
        '#type' => 'select',
        '#title' => 'Counter Type',
        '#description' => 'Select the counter type to add',
        '#options' => $options,
    );

    $form['value'] = array(
        '#type' => 'textfield',
        '#title' => 'Value',
        '#description' => 'Enter the new value for this counter',
        '#default_value' => '0',
    );

    $date_str = date( 'Y-m-d', time());
    $form['expire'] = array(
        '#type' => 'date',
        '#title' => 'Expire Date',
        '#description' => 'Enter the new expiration date',
        '#default_value' => date_parse( $date_str),
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
function ocs_admin_counter_add_form_submit( $form, &$form_state) 
{
//    dpm( $form_state['values'], 'values');
    $values = $form_state['values'];

    if ( $values['op'] == 'Save') {

        $counter_name = $form['#all_counter'];
        $counter_id = $values['type'];
        $counter_scale = $counter_name[$counter_id]['counter_scale'];

        $counter = array( 
            'subscription_key' => $values['subscription_key'],
            'counter_id' => $counter_id,
            );

        $counter['counter_value'] = $values['value'];
        if ($counter_scale > 0 ) {
            $counter['counter_value'] *= $counter_scale;
        }
        
        $date = $values['expire'];
        $counter['counter_expire'] = strtotime( sprintf( '%04d-%02d-%02d', $date['year'] , $date['month'] , $date['day'])) * 1000;
        dpm( $counter, 'save counter');

        ocs_admin_counter_save( $counter);
    }
    
    $form_state['redirect'] = 'ocs_admin/account/all/' . $form_state['values']['account_key'];
}


