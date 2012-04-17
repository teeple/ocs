<?php

/*
** generate for for ocs_account/add page

    3 possible implementation

    ocs_admin_account_add_form_popup : popup version
    ocs_admin_account_add_form_panel : panel version
    ocs_admin_account_add_form_innertable : inner table version
*/
function ocs_admin_account_add_page( $impl = NULL, $arg = NULL, $init = TRUE) 
{
    dpm( $impl, 'ocs_admin_account_add_page');
    if ( $arg ) {
        foreach( $arg as $key => $value) {
            $_SESSION['ocs_admin_account_add'][$key] = $value;
        }
    }
    else {
        $_SESSION['ocs_admin_account_add'] = array();
    }

    $form = drupal_get_form( 'ocs_admin_account_add_form'.$impl , $_SESSION['ocs_admin_account_add']);
    $output = '<div id="ocs_admin_account_add_form_div">' . drupal_render( $form) . '</div>';

    return $output;
}


/*
** PHP function that shall be called in the view block
*/
function ocs_admin_account_select_row( $args)
{
    ctools_include('ajax'); // Module  include the dependence it needs for ajax.
    $link = ctools_ajax_text_button( t('select'), current_path()."/nojs/".$args, t('select'));
    return $link;
}

/*
**  ajax select button in account & product table
    impl: _panel, _popup, and etc
*/
function ocs_account_select( $impl, $js = NULL, $type = 'customer', $id = NULL, $name = NULL)
{
    $arg = array( 
        $type . '_id' => $id, 
        $type . '_name' => $name,
        ); 

    $output = ocs_admin_account_add_page( $impl, $arg);

    if ( $js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace( '#ocs_admin_account_add_form_div', $output);
        print ajax_render( $commands);
        exit;
    }
    else {
        return $output;
    }
}



   /* table example
   $header = array(
        array(
            'data' => 'Attributes',
            'width' => 100,
        ),
        array(
            'data' => 'Values',
            'colspan' => 2,
        ),
    );

   $rows = array(
        array(
            array( 'data' => 'Customer', 'rowspan' => 2),
            array( 'data' => 'ID', 'width' => 100), 
            $customer_id,
        ),
        array( 'Name', 
            $customer_name
            ),
   );

    $form['table'] = array(
        '#type' => 'markup',
        '#markup' => theme( 'table', array( 'header' => $header, 'rows' => $rows)),
        '#prefix' => '<div id="ocs_admin_account_customer_form_div">',
        '#suffix' => '</div>',
    );
    */

function ocs_admin_account_add_form_panel( $form, &$form_state, $arg=NULL)
{
    $form = array();

    $form['customer'] = array(
        '#type' => 'fieldset',
        '#title' => t('Customer'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#prefix' => '<div id="ocs_admin_account_customer_form_div">',
        '#suffix' => '</div>',
    );

    $form['customer']['customer_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer ID'),
        '#default_value' => isset($arg['customer_id']) ? $arg['customer_id'] : '',
        '#required' => TRUE,
        '#description' => t('<a href="#@link">Select a customer</a> to add an account', 
            array( '@link' => 'select_customer')),
        '#size' => '30',
    );

    $form['customer']['customer_name'] = array(
        '#type' => 'item',
        '#title' => t('Name : @name', 
            array('@name' => isset($arg['customer_name']) ? $arg['customer_name'] : '')),
    );

    $form['product'] = array(
        '#type' => 'fieldset',
        '#title' => t('Product'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#prefix' => '<div id="ocs_admin_account_product_form_div">',
        '#suffix' => '</div>',
    );

    $form['product']['product_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Product ID'),
        '#default_value' => isset($arg['product_id']) ? $arg['product_id'] : '',
        '#required' => TRUE,
        '#description' => t('<a href="#@link">Select a product</a> to assign', 
            array( '@link' => 'select_product')),
        '#size' => '30',
    );

    $form['product']['product_name'] = array(
        '#type' => 'item',
        '#title' => t('Name : @name', 
            array('@name' => isset($arg['product_name']) ? $arg['product_name'] : '')),
    );

    $form['account_key'] = array(
        '#type' => 'fieldset',
        '#title' => t('Account Key (MSISDN)'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#prefix' => '<div id="ocs_admin_account_key_form_div">',
        '#suffix' => '</div>',
    );

    $form['account_key']['account_key'] = array(
        '#type' => 'textfield',
        '#title' => t('Account Key'),
        '#description' => t('Key of Account such as MSISDN or User Name'),
        '#default_value' => isset($arg['key_id']) ? $arg['key_id'] : '',
        '#required' => TRUE,
        '#description' => t('<a href="#@link">Select an account key</a> to assign', 
            array( '@link' => 'select_key')),
        '#size' => '30',
    );

    $form['account_key']['key_type'] = array(
        '#type' => 'item',
        '#title' => t('Account Type : @name', 
            array('@name' => isset($arg['key_name']) ? $arg['key_name'] : '')),
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Confirm'),
        '#submit' => array('ocs_admin_account_add_form_panel_submit'),
    );

    $form['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
        '#submit' => array('ocs_admin_account_add_form_panel_submit'),
    );

//    dpm( $form_state, 'form state: ');
//    ddebug_backtrace();

    return $form;
}

function ocs_admin_account_add_form_panel_validate( $form, &$form_state)
{
//    dpm( $form_state, 'validate: ');

    // check parameters
    $account_key = ocs_admin_account_key_load( $form_state['values']['account_key']);
    if ( ! $account_key) {
        form_set_error( 'account_key', 'Invalid account key');
    }

    $customer_id = ocs_admin_customer_load( $form_state['values']['customer_id']);
    if ( ! $customer_id) {
        form_set_error( 'customer_id', 'Invalid customer ID');
    }

    $product_id = ocs_admin_product_load( $form_state['values']['product_id']);
    if ( ! $product_id) {
        form_set_error( 'product_id', 'Invalid product ID');
    }
}

function ocs_admin_account_add_form_panel_submit( $form, &$form_state)
{
    $form_state['redirect'] = 'ocs_admin/account';
    dpm( $form_state, 'submit: ');

    if ( $form_state['values']['op'] == 'Confirm') {
        // save to DB
        dsm( 'confirmed: ');
        if ( ocs_admin_account_add( $form_state['values'])) {
            drupal_set_message( 'A new account is created.');
        }
    }
}


function ocs_admin_form_ocs_admin_account_add_form_panel_alter( &$form, &$form_state, $form_id) 
{
    $form['#action'] = url('ocs_admin/account/add/_panel');
}

/*
** save to DB
*/
/*
function ocs_admin_db_account_add( $values) 
{
    db_set_active('ocsdb');

    $txn = db_transaction();

    $fields = array(
        'customer_id' => $values['customer_id'],
        'product_id' => $values['product_id'],
    );

    try {
        $id = db_insert('ocs_account')
            ->fields( $fields)
            ->execute();
        dpm( $id, 'db insert');

        return $id;
    }
    catch( Exception $e) {
        $txn->rollback();
        // Log the exception to watchdog.
        watchdog_exception('type', $e);
        dpm( $e, 'db insert failed');
    }

    db_set_active('default');
 }
 */

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

    ////////// account key : MSISDN

    $form['account'] = array(
        '#type' => 'fieldset',
        '#title' => t('Account'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['account']['account_key'] = array(
        '#type' => 'textfield',
        '#title' => t('Account Key'),
        '#ajax' => array(
            'callback' => 'ocs_admin_db_account_callback',
            'wrapper' => 'replace_account_list_div',
            'keypress' => TRUE,
        ),
     );

    $form['account']['account_list'] = ocs_admin_get_table_form( 'account');

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#submit' => array( 'ocs_admin_account_add_submit'),
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

    if ( !empty( $form_state['values']['account_key'])) {
        $arg = array(
            'key' => $form_state['values']['account_key'],
            );
        $form['account']['account_list'] = ocs_admin_get_table_form( 'account', $arg);
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
    else if ( $type == 'product') {
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
   else if ( $type == 'account') {
         $header = array(
            'account_key' => 'Key',
        );
        $records = ocs_admin_get_account( $arg);

        $form = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $records,
            '#empty' => t('No content available.'),
            '#prefix' => '<div id="replace_account_list_div">',
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

function ocs_admin_get_account( $arg = NULL)
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

function ocs_admin_db_account_callback( $form, $form_state)
{
    return $form['account']['account_list'];
}

/*
** submit 'account/add'
*/
function ocs_admin_account_add_submit( $form, &$form_state)
{
    dsm( $form_state['values']);
}

/***************************************************
** not used
*/

function ocs_admin_account_customer_form_popup( $arg = NULL)
{
    // Include the CTools tools that we need.
    ctools_include('ajax');
    ctools_include('modal');

    // Add CTools' javascript to the page.
    ctools_modal_add_js();

    $form['customer'] = ocs_admin_account_customer_form_panel( $arg);

    $form['customer']['list'] = array(
        '#type' => 'markup',
        '#markup' => ctools_modal_text_button(t('Search'), 'helloworld/nojs/form', t('Search customer')),
        );

    return $form;
}

function ocs_admin_account_product_form_popup( $arg = NULL)
{
    return  ocs_admin_account_product_form_panel( $arg);
}

/*
** POP up version
*/
function ocs_admin_account_add_form_popup( $form, &$form_state, $arg = NULL) 
{
    $form['customer'] = ocs_admin_account_customer_form_popup( arg);
    $form['product'] = ocs_admin_account_product_form_popup( arg);

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

/*
** delete an account
*/
function ocs_admin_account_delete_confirm( $form, &$form_state, $account_key)
{
    $form['#account_key'] = $account_key;
    $caption = '<p>' . 
            t('<strong>Warning:</strong> The account "%key" will be deleted.',
                array( '%key' => $account_key)) .
            '</p>';

    $caption .= '<p>'. t('This action cannot be undone.') . '</p>';

    return confirm_form( $form, t('Are you sure to delete the account "%key"?',
        array( '%key' => $account_key)),
        'ocs_admin/account',
        $caption, t('Delete'));
}

function ocs_admin_account_delete_confirm_submit( $form, &$form_state)
{
    $account_key = $form['#account_key'];
    ocs_admin_account_delete( $account_key);

    $t_args = array( '%key' => $account_key);
    drupal_set_message( t('The account %key has been deleted.', $t_args));
    watchdog( 'ocs_admin', 'Deleted the account %key.', $t_args, WATCHDOG_NOTICE);

    $form_state['redirect'] = 'ocs_admin/account';
}

function ocs_admin_account_edit_form( $form, &$form_state, $account_key)
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
        '#value' => $account_key,
    );

    $subs = ocs_admin_subscription_load( $account_key);
    $form['#subscription'] = $subs;

    $form['subscription_type'] = array(
        '#type' => 'textfield',
        '#title' => 'Subscription Type',
        '#description' => 'Enter the subscription type',
        '#default_value' => $subs['subscription_type'],
    );

    $form['charging_type'] = array(
        '#type' => 'textfield',
        '#title' => 'Charging Type',
        '#description' => 'Enter the charging type',
        '#default_value' => $subs['charging_type'],
    );

    $form['active_expire_date'] = array(
        '#type' => 'date',
        '#title' => 'Active Status Expire Date',
        '#description' => 'Enter the new expiration date',
        '#default_value' => date_parse( $subs['active_expire_date']),
    );

    $form['grace_expire_date'] = array(
        '#type' => 'date',
        '#title' => 'Grace Status Expire Date',
        '#description' => 'Enter the new expiration date',
        '#default_value' => date_parse( $subs['grace_expire_date']),
    );

    $form['notification_flag_set'] = array(
        '#type' => 'textfield',
        '#title' => 'Notification Flag',
        '#description' => 'Enter the new Notification Flag',
        '#default_value' => $subs['notification_flag_set'],
    );

    $form['preferred_language'] = array(
        '#type' => 'textfield',
        '#title' => 'Preferred Language',
        '#description' => 'Select preferred language type',
        '#default_value' => $subs['preferred_language'],
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
    submit account info
*/
function ocs_admin_account_edit_form_submit( $form, &$form_state) 
{
    if ( $form_state['values']['op'] == 'Save') {
        $subs = $form['#subscription'];

        // update changed value
        foreach( $form_state['values'] as $key => $value) {
            if ( isset( $subs[ $key ])) {
                if ( $form[ $key]['#type'] == 'date') {
                    // date format
                    $subs[ $key] = sprintf( '%04d-%02d-%02d', $value['year'], $value['month'], $value['day']);
                }
                else {
                    $subs[ $key] = $value;
                }
            }
        }

        ocs_admin_subscription_save( $subs);
    }
    
    $form_state['redirect'] = 'ocs_admin/account/all/' . $form_state['values']['account_key'];
}

/*
load counter
*/
function ocs_admin_subscription_load( $subscription_key)
{
    db_set_active('ocsdb');

    $query = db_select( 'ocs_subscription', 'a');
    $result = $query->fields( 'a')
                    ->condition( 'subscription_key', $subscription_key, '=')
                    ->execute()
                    ->fetchAssoc();
   
    db_set_active('default');

    return $result;
}

/*
save counter
*/
function ocs_admin_subscription_save( $data)
{
    db_set_active('ocsdb');

    $result = db_update( 'ocs_subscription')
        ->fields( $data)
        ->condition( 'subscription_key', $data['subscription_key'], '=')
        ->execute();
    dpm( $result, 'db_update ocs_counter result');

    db_set_active('default');
    
    return $result;
}


/*
    save OCS Customer Account
*/
function ocs_admin_account_add( $account)
{
    dpm( debug_backtrace(), 'backtrace');

    if ( ! isset($account['account_key']) || ! isset($account['customer_id'])  ||
        ! isset($account['product_id'])) {
        drupal_set_message( 'Insufficient parameters to create an account', 'warning');
        return FALSE;
    }

    db_set_active( 'ocsdb');

    $txn = db_transaction();

    try {
        // insert record in ocs_subscription table
        $default_value = array(
            'subscription_key' => $account['account_key'],
            'subscription_type' => 0,
            'charging_type' => 0,
            'status' => 0,
            'active_expire_date' => date("Y-m-d H:i:s", time()),
            'grace_expire_date' => date("Y-m-d H:i:s", time()),
            'home_info' => '',
            'preferred_language' => 0,
            'notification_flag_set' => 0,
        );

        $result = db_insert( 'ocs_subscription')
            ->fields( $default_value)
            ->execute();
        dpm( $result, 'db_insert ocs_subscription result');

        // insert record in ocs_account table
        $fields = array( 
            'account_key' => $account['account_key'],
            'status' => 0,
            'subscription_key' => $account['account_key'],
            'customer_id' => $account['customer_id'],
            'product_id' => $account['product_id'],
            );

        $status = db_insert( 'ocs_account')
            ->fields( $fields)
            ->execute();
        dpm( $status, 'db_insert ocs_account result');

        // insert record in ocs_counter_tbl
        // set account_key as used
        $result = _ocs_admin_account_key_save( $account['account_key'], array(
                'sales_flag' => '1',
            ));

    } 
    catch( Exception $e) {
        $txn->rollback();

        dpm( $e, 'DB transaction failed');
        watchdog_exception( 'ocs_admin', $e);
    }

    db_set_active( 'default');
}

function _ocs_admin_account_key_save( $account_key, $fields)
{
    // update account key as used
    $result = db_update( 'ocs_account_key')
        ->fields( $fields)
        ->condition( 'account_key', $account_key, '=')
        ->execute();
    dpm( $result, 'db_update ocs_account_key result');
}

/*
    delete account
*/
function ocs_admin_account_delete( $account_key)
{
    db_set_active( 'ocsdb');

    $txn = db_transaction();

    try {
        // delete ocs_account
        $result = db_delete( 'ocs_account')
            ->condition( 'account_key', $account_key, '=')
            ->execute();
        dpm( $result, 'ocs_account result');

        $result = db_delete( 'ocs_subscription')
            ->condition( 'subscription_key', $account_key, '=')
            ->execute();
        dpm( $result, 'ocs_subscription result');

        // set account_key as unused
        $result = _ocs_admin_account_key_save( $account_key, array(
                'sales_flag' => '0',
            ));

    } 
    catch( Exception $e) {
        $txn->rollback();

        watchdog_exception( 'ocs_admin', $e);
    }

    db_set_active( 'default');
}


/*
    load customer info
*/
function ocs_admin_customer_load( $customer_id)
{
   db_set_active('ocsdb');

   $query = db_select( 'ocs_customer', 'g');
   $result = $query->fields( 'g')
        ->condition( 'customer_id', $customer_id, '=')
        ->execute() 
        ->fetchAssoc();
//    dpm( $result, 'fetched customer');

    db_set_active('default');

    return $result;
}

/*
    load account key info
*/
function ocs_admin_account_key_load( $account_key)
{
   db_set_active('ocsdb');

   $query = db_select( 'ocs_account_key', 'g');
   $result = $query->fields( 'g')
        ->condition( 'account_key', $account_key, '=')
        ->execute()
        ->fetchAssoc();
//    dpm( $result, 'fetched account key');

    db_set_active('default');

    return $result;
}


