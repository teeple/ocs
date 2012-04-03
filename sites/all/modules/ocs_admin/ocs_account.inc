<?php

/*
** generate for for ocs_account/add page
*/
function ocs_admin_account_add() 
{
    $form = drupal_get_form( 'ocs_admin_account_add_form');
    $output = '<div id="ocs_admin_account_add_form">' .  drupal_render( $form) . '</div>';

    return $output;
}


function ocs_admin_account_add_form2( $form, &$form_state, $arg = NULL) 
{
    $form['customer'] = array(
        '#type' => 'fieldset',
        '#title' => t('Customer'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['customer']['id'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer ID'),
        '#default_value' => $arg['customer_id'],
    );

    $form['customer']['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer Name'),
        '#default_value' => $arg['customer_name'],
    );

    $form['product'] = array(
        '#type' => 'fieldset',
        '#title' => t('Product'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['product']['id'] = array(
        '#type' => 'textfield',
        '#title' => t('Product ID'),
        '#default_value' => $arg['product_id'],
    );

    $form['product']['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Product Name'),
        '#default_value' => $arg['product_name'],
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#submit' => array( 'ocs_admin_account_add_form_submit'),
    );

    return $form;
}

function ocs_admin_account_add_form_submit( $form, &$form_state) 
{
    kpr( $form_state, TRUE);
}

function ocs_admin_account_select_row( $args)
{
    ctools_include('ajax'); // Module  include the dependence it needs for ajax.
    $link = ctools_ajax_text_button( t('select'), "account/nojs/select/".$args, t('select'));
    return $link;
}

/*
**  ajax select button in account & product table
*/
function ocs_admin_account_select( $js = NULL, $type = 'customer', $id = NULL, $name = NULL)
{
    if ( ! isset( $_SESSION['account_select'])) {
        $_SESSION['account_select'] = array();
    }

    /* store variables in session variable */
    $_SESSION['account_select'][$type . '_id'] = $id;
    $_SESSION['account_select'][$type . '_name'] = $name;

    $form = drupal_get_form( 'ocs_admin_account_add_form', $_SESSION['account_select']);

    $output = '<div id="ocs_admin_account_add_form">' . drupal_render( $form) . '</div>';
    if ( $js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace( '#ocs_admin_account_add_form', $output);
        print ajax_render( $commands);
        exit;
    }
    else {
        return $output;
    }
}



/* 
** for testing form building
*/
function ocs_account_add_form( $form, &$form_state)
{
    $form['customer'] = array(
        '#type' => 'fieldset',
        '#title' => t('Customer'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['customer']['customer_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer ID'),
    );

    $form['customer']['customer_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer Name'),
        '#ajax' => array(
            'callback' => 'ocs_admin_account_add_form_callback',
            'wrapper' => 'replace_customer_list_div',
            'keypress' => TRUE,
        ),
    );

    $form['customer']['list'] = ocs_admin_account_get_table();

    /*
    $form['customer']['list'] = array(
        '#type' => 'markup',
        '#markup' => 'before',
        '#prefix' => '<div id="replace_customer_list_div">',
        '#suffix' => '</div>',
    );
    */

    $form['product'] = array(
        '#type' => 'fieldset',
        '#title' => t('Product'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['product']['id'] = array(
        '#type' => 'textfield',
        '#title' => t('Product ID'),
    );

    $form['product']['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Product Name'),
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
    );

    if ( !empty( $form_state['values']['customer_name'])) {
        $form['customer']['list'] = ocs_admin_account_get_table( $form_state['values']['customer_name']);
    }

    return $form;
}

function ocs_admin_account_get_table( $name = NULL) 
{
    // read records from customer table
    $records = ocs_admin_get_customer( array( 'name' => $name));
    
    $header = array( 
        'customer_id' => 'ID', 
        'customer_name' => 'Name');

    return array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $records,
        '#empty' => t('No content available.'),
        '#prefix' => '<div id="replace_customer_list_div">',
        '#suffix' => '</div>',
    );
}

function ocs_admin_get_customer( $arg)
{
    db_set_active('ocsdb');
    
    $header = array( 'customer_id', 'customer_name');
    $query = db_select( 'ocs_customer', 'c');
    if ( isset( $arg['name'])) {
        $query->condition( 'c.customer_name', $arg['name'], 'like');
    }
    $query
        ->fields( 'c', $header)
        ->range( 0, 10);

    $result = $query->execute();
    $records = array();
    while( $record = $result->fetchAssoc()) {
        $records[$record['customer_id']] = $record;
    }

    db_set_active('default');

    return $records;
}


/*
** ajax callback 
*/
function ocs_admin_account_add_form_callback( $form, $form_state)
{
    return $form['customer']['list'];
}
