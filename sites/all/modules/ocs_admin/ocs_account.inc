<?php

/*
** generate for for ocs_account/add page
*/
function ocs_admin_account_add( $arg = NULL) 
{
    $form = drupal_get_form( 'ocs_admin_account_add_form' . $arg);
    $output = '<div id="ocs_admin_account_add_form">' .  drupal_render( $form) . '</div>';

    return $output;
}


/*
** POP up version
*/
function ocs_admin_account_add_form_popup( $form, &$form_state, $arg = NULL) 
{
    // Include the CTools tools that we need.
    ctools_include('ajax');
    ctools_include('modal');

    // Add CTools' javascript to the page.
    ctools_modal_add_js();

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

    $form['customer']['list'] = array(
        '#type' => 'markup',
        '#markup' => ctools_modal_text_button(t('Search'), 'helloworld/nojs/form', t('Search customer')),
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
    $link = ctools_ajax_text_button( t('select'), "ocs_admin/nojs/select/".$args, t('select'));
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
** add form: two column version
*/
function ocs_admin_account_add_form( $form, &$form_state)
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
        '#size' => 30,
    );

    $form['customer']['customer_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer Name'),
        '#size' => 30,
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
        '#size' => 30,
    );

    $form['product']['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Product Name'),
        '#size' => 30,
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


/* 
** build form for adding account
** inner table version
*/
function ocs_admin_account_add_form_innertable( $form, &$form_state)
{
    ///////////////////////// customer list

    $form = array();
    $form['customer'] = array(
        '#type' => 'fieldset',
        '#title' => t('Customer'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['customer']['customer_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer ID'),
        '#ajax' => array(
            'callback' => 'ocs_admin_db_customer_callback',
            'wrapper' => 'replace_customer_list_div',
            'keypress' => TRUE,
        ),
    );

    $form['customer']['customer_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer Name'),
        '#ajax' => array(
            'callback' => 'ocs_admin_db_customer_callback',
            'wrapper' => 'replace_customer_list_div',
            'keypress' => TRUE,
        ),
    );

    $form['customer']['customer_list'] = ocs_admin_get_table_form( 'customer');

    ////////// product list

    $form['product'] = array(
        '#type' => 'fieldset',
        '#title' => t('Product'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['product']['product_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Product ID'),
        '#ajax' => array(
            'callback' => 'ocs_admin_db_product_callback',
            'wrapper' => 'replace_product_list_div',
            'keypress' => TRUE,
        ),
     );

    $form['product']['product_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Product Name'),
        '#ajax' => array(
            'callback' => 'ocs_admin_db_product_callback',
            'wrapper' => 'replace_product_list_div',
            'keypress' => TRUE,
        ),
    );

    $form['product']['product_list'] = ocs_admin_get_table_form( 'product');

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
    );

    if ( !empty( $form_state['values']['customer_name']) || ! empty( $form_state['values']['customer_id'])) {
        $arg = array(
            'name' => $form_state['values']['customer_name'],
            'id' => $form_state['values']['customer_id'],
            );
        $form['customer']['customer_list'] = ocs_admin_get_table_form( 'customer', $arg);
    }

    if ( !empty( $form_state['values']['product_name']) || ! empty( $form_state['values']['product_id'])) {
        $arg = array(
            'name' => $form_state['values']['product_name'],
            'id' => $form_state['values']['product_id'],
            );
        $form['product']['product_list'] = ocs_admin_get_table_form( 'product', $arg);
    }

    return $form;
}

function ocs_admin_get_table_form( $type, $arg = NULL)
{
    if ( $type == 'customer') {
        $header = array(
            'customer_id' => 'ID',
            'customer_name' => 'Name',
        );
        $records = ocs_admin_get_customer( $arg);

        $form =  array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $records,
            '#empty' => t('No content available.'),
            '#prefix' => '<div id="replace_customer_list_div">',
            '#suffix' => '</div>',
        );
    }
    else {
         $header = array(
            'product_id' => 'ID',
            'product_name' => 'Name',
        );
        $records = ocs_admin_get_product( $arg);

        $form = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $records,
            '#empty' => t('No content available.'),
            '#prefix' => '<div id="replace_product_list_div">',
            '#suffix' => '</div>',
        );
   }

   return $form;
}

function ocs_admin_get_customer( $arg = NULL)
{
    db_set_active('ocsdb');
    
    $header = array( 'customer_id', 'customer_name');
    $query = db_select( 'ocs_customer', 'c');
    if ( isset( $arg['name'])) {
        $query->condition( 'c.customer_name', '%'.$arg['name'].'%', 'like');
    }
    if ( isset( $arg['id'])) {
        $query->condition( 'c.customer_id', $arg['id'], '=');
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

function ocs_admin_get_product( $arg = NULL)
{
    db_set_active('ocsdb');
    
    $header = array( 'product_id', 'product_name');
    $query = db_select( 'product', 'p');
    if ( isset( $arg['name'])) {
        $query->condition( 'p.product_name', '%'.$arg['name'].'%', 'like');
    }
    if ( isset( $arg['id'])) {
        $query->condition( 'p.product_id', $arg['id'], '=');
    }
    $query
        ->fields( 'p', $header)
        ->range( 0, 10);

    $result = $query->execute();
    $records = array();
    while( $record = $result->fetchAssoc()) {
        $records[$record['product_id']] = $record;
    }

    db_set_active('default');

    return $records;
}


/*
** ajax callback 
*/
function ocs_admin_db_customer_callback( $form, $form_state)
{
    return $form['customer']['customer_list'];
}

function ocs_admin_db_product_callback( $form, $form_state)
{
    return $form['product']['product_list'];
}
