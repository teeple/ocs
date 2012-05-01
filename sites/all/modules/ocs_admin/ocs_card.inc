<?php


/*
** add/edit card type information form
*/
function ocs_admin_card_form( $form, &$form_state, $type, $cardtype)
{    
    $cardtype = cardtype_load($cardtype);

    $form['card_type'] = array(
        '#type' => 'hidden',
        '#value' => ($type == 'edit') ? $cardtype['card_type'] : NULL,
    );

    $form['card_type'] = array(
        '#type' => 'textfield',
        '#title' => 'Card Type',
        '#maxlength' => 4,
        '#size' => 4,
        '#description' => t('Enter card type'),
        '#default_value' => ($type == 'edit') ? $cardtype['card_type'] : NULL,
    );

    $form['unit'] = array(
        '#type' => 'textfield',
        '#title' => 'Card Unit',
        '#maxlength' => 5,
        '#size' => 5,
        '#description' => t('Enter card unit'),
        '#default_value' => ($type == 'edit') ? $cardtype['unit'] : NULL,
    );

    $form['active_period'] = array(
        '#type' => 'textfield',
        '#title' => 'Active Period',
        '#maxlength' => 5,
        '#size' => 5,
        '#description' => t('Enter active period'),
        '#default_value' => ($type == 'edit') ? $cardtype['active_period'] : NULL,
    );

    $form['grace_period'] = array(
        '#type' => 'textfield',
        '#title' => 'Grace Period',
        '#maxlength' => 5,
        '#size' => 5,
        '#description' => t('Enter grace period'),
        '#default_value' => ($type == 'edit') ? $cardtype['grace_period'] : NULL,
    );

    $form['description'] = array(
        '#type' => 'textfield',
        '#title' => 'Description',
        '#maxlength' => 128,
        '#size' => 128,
        '#description' => t('Enter card type descripton'),
        '#default_value' => ($type == 'edit') ? $cardtype['description'] : NULL,
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

    return $form;
}

function ocs_admin_card_form_validate( $form, &$form_state)
{
    $cardtype = $form_state['values'];
    dpm($cardtype, 'card type');
    if ( is_numeric( $cardtype['card_type'])) {
         form_set_error('card type', t('You have to enter string values the card type.'));
    }
    
    if ( ! is_numeric( $cardtype['unit'])) {
         form_set_error('unit', t('You have to enter numeric values the unit.'));
    }

    if ( ! is_numeric( $cardtype['active_period'])) {
         form_set_error('active period', t('You have to enter numeric values the active period.'));
    }

    if ( ! is_numeric( $cardtype['grace_period'])) {
         form_set_error('grace period', t('You have to enter numeric values the grace period.'));
    }
}


function ocs_admin_card_form_submit( $form, &$form_state )
{
    $cardtype = $form_state['values'];

    if (ocs_admin_card_form_save($cardtype) != 0 ) {
	    drupal_set_message( t('Your configuration has been saved.'));
	    $form_state['redirect'] = '/ocs_admin/cardtype';
    } else {
  
 	    $form['unit'] = array('#type' => 'textfield', '#value' => $cardtype['unit']);
	    $form['active_period'] = array('#value' => $cardtype['active_period']);
	    $form['grace_period'] = array('#value' => $cardtype['grace_period']);
	    $form['description'] = array('#value' => $cardtype['description']);
    }
}

/*
    get subgroup list
*/
function ocs_admin_card_form_get_data( $cardtype )
{
    db_set_active('ocsdb');

    // get group information
    $query = db_select( 'ocs_card_type', 'a');
    $result = $query->condition( 'a.card_type', $cardtype, '=')
        ->fields( 'a')
        ->execute()
        ->fetchAllAssoc( 'card_type', PDO::FETCH_ASSOC);

    db_set_active( 'default');

    return $result;
}

function ocs_admin_card_form_product_card_edit( $form, &$form_state, $type, $product_card)
{    
//    dpm($type, 'ocs_card_type');

    $form['product_card'] = array(
        '#type' => 'hidden',
        '#value' => ($type == 'edit') ? $product_card['card_type'] : NULL,
    );

    $form['card_type'] = array(
        '#type' => 'textfield',
        '#title' => 'Card Type',
        '#maxlength' => 4,
        '#size' => 4,
        '#description' => t('Select card type'),
        '#default_value' => ($type == 'edit') ? $product_card['card_type'] : NULL,
    );

    $form['product_id'] = array(
        '#type' => 'textfield',
        '#title' => 'Product ID',
        '#maxlength' => 45,
        '#size' => 45,
        '#description' => t('Select Product ID'),
        '#default_value' => ($type == 'edit') ? $product_card['product_id'] : NULL,
    );

    $form['usable'] = array(
        '#type' => 'textfield',
        '#title' => 'Usable',
        '#maxlength' => 1,
        '#size' => 1,
        '#description' => t('Select Usable'),
        '#default_value' => ($type == 'edit') ? $product_card['usable'] : NULL,
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

    return $form;
}

function ocs_admin_card_form_add_panel( $form, &$form_state, $arg=NULL)
{
    dpm($arg, 'add_panel');
    $from = array();
    $form['product_card'] = array(
        '#type' => 'fieldset',
        '#title' => t('Connnect product and card type'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#prefix' => '<div id="ocs_admin_product_card_div">',
        '#suffix' => '</div>'
        );

    $form['product_card']['card_type'] = array(
        '#type' => 'textfield',
        '#title' => t('Card Type'),
        '#default_value' => isset($arg['card_type']) ? $arg['card_type'] : NULL,
        '#description' => t('<a href="#@link">Select a card type</a> to add assign',
            array('@link' => 'Card Type')),
        '#size' => '30',
        );

     $form['product_card']['description'] = array(
        '#type' => 'item',
        '#title' => t('Description : @description',
            array('@description' => isset($arg['card_description']) ? $arg['card_description'] : '')),
        );

     $form['product_card']['product_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Product'),
        '#default_value' => isset($arg['product_id']) ? $arg['product_id'] : NULL,
        '#description' => t('<a href="#@link">Select a product</a> to add assign',
            array('@link' => 'Product Name')),
        '#size' => '30',
        );

     $form['product_card']['product_name'] = array(
        '#type' => 'item',
        '#title' => t('Product Name : @product_name',
            array('@product_name' => isset($arg['product_name']) ? $arg['product_name'] : '')),
        );

     $form['product_card']['usable'] = array(
        '#type' => 'select',
        '#title' => t('Usable'),
        '#options' => array(
            0 => t('Unusable'),
            1 => t('Usable'),
            ),
        '#default_value' => isset($arg['usable']) ? $arg['usable'] : '1',
        '#description' => t('Set this to <em>Usable</em> if you would like this category to be selected by default.'),
        );

    $form['actions'] = array('#type' => 'actions');
 
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    
    $form['actions']['Cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
    );


    return $form;
}

function ocs_admin_card_pct_add_page( $impl = NULL, $arg = NULL, $init = TRUE)
{
    $impl = '_wizard';
    if (! isset($_SESSION['ocs_admin_productcard_save'])) {
        $_SESSION['ocs_admin_productcard_save'] = array();
    }
    $_SESSION['ocs_admin_productcard_save'] = array_merge($_SESSION['ocs_admin_productcard_save'], $_GET);
    
    $form = drupal_get_form('ocs_admin_productcard'.$impl, $_SESSION['ocs_admin_productcard_save']);
    $output = '<div id="ocs_admin_productcard_div">' . drupal_render( $form) . '</div>';
    
    return $output;
}

function  ocs_admin_card_form_select_row($args)
{
    ctools_include('ajax'); 
    $link = ctools_ajax_text_button(t('select'), current_path()."/nojs/".$args, t('select'));
    return $link;
}

function ocs_cardtype_select($impl, $js=NULL, $type='card', $id=NULL, $name=NULL)
{
    $arg = array(
        $type . '_type' => $id,
        $type . '_description' => $name,
        );
    
    $output = ocs_admin_card_pct_add_page($impl, $arg);
    
    if ($js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace('#ocs_admin_card_form_add_div', $output);
        print ajax_render($commands);
        exit;
    }
    else {
        return $output;
    }
}

function ocs_product_select($impl, $js=NULL, $type='product', $id=NULL, $name=NULL)
{
    $arg = array(
        $type . '_id' => $id,
        $type . '_name' => $name,
        );
    
    $output = ocs_admin_card_form_add_page($impl, $arg);

    if ($js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace('#ocs_admin_card_form_add_div', $output);
        print ajax_render($commands);
        exit;
    }
    else {
        return $output;
    }
}

function ocs_productcard_select($impl, $js=NULL, $type='productcard', $id=NULL, $name=NULL)
{
    $arg = array(
        $type . '_id' => $id,
        $type . '_name' => $name,
        );
    $output = ocs_admin_card_form_add_page($impl, $arg);

    if ($js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace('#ocs_admin_card_form_add_div', $output);
        print ajax_render($commands);
        exit;
    }
    else {
        return $output;
    }
}

function ocs_admin_card_form_add_panel_submit($form, &$form_state)
{
    if ($form_state['values']['op'] == 'Save') {
        $form_state['redirect'] = 'ocs_admin/product/card';
        if (ocs_admin_productcard_save ($form_state['values'])) {
            drupal_set_message('Connected product and cardtype');    
        }
    } else if ($form_state['values']['op'] == 'Cancel') {
        $_SESSION['product_card'] = array();
    }
}

function ocs_admin_productcard_save($cardtype)
{
    if ( ! isset($cardtype['card_type']) || ! isset($cardtype['product_id'])) {
        drupal_set_message( 'Insufficient parameters to connect product and cardtype', 'warning');
        return FALSE;
    }

    db_set_active('ocsdb');

    $txn = db_transaction();

    try {
        $default_value = array (
            'card_type' => $cardtype['card_type'],
            'product_id' => $cardtype['product_id'],
            'usable' => $cardtype['usable'],
            );

        $result = db_insert('ocs_product_card')
            ->fields($default_value)
            ->execute();
    }
    catch(Exception $e){
        try {
            $default_value = array();
            $default_value = array(
                'usable' => $cardtype['usable']
                );
            $result = db_update('ocs_product_card')
                ->fields($default_value)
                ->condition('card_type', $cardtype['card_type'], '=')
                ->condition('product_id', $cardtype['product_id'], '=')
                ->execute();
            }
            catch(exception $e){
                $txn->rollback();
                dpm($e, 'db transaction failed');
                watchdog_exception('ocs_admin', $e);
            }
    }
    db_set_active('default');
}

function ocs_admin_productcard_wizard( $form, &$form_state, $arg = NULL)
{
    dpm($arg, 'ocs_admin_productcard_wizard');
    /* Card Type */
    $form = array();
    $form['section'] = array(
        '#type' => 'fieldset',
        '#description' => 'Input parameters to connect a card type and a product.',
    );

    $form['section']['product'] = array(
        '#type' => 'fieldset',
        '#title' => t('Product'),
//        '#collapsible'=> TRUE,
//        '#collapsed' => FALSE,
        '#description' => 'Click the <em>Product</em> button to find a product.',
        '#attributes' => array('class' => array('column')),
    );

     $form['section']['product']['product_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Product ID'),
        '#size' => '30',
    );


    $form['section']['product']['product_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Product Name'),
        '#size' => '30',
    );
    
    $form['section']['product']['product_search'] = array(
        '#type' => 'submit',
        '#value' => 'Search Product',
    );


    $form['section']['cardtype'] = array(
        '#type' => 'fieldset',
        '#title' => t('Card Type'),
//        '#collapsible'=> TRUE,
//        '#collapsed' => FALSE,
        '#description' => 'Click the <em>Card Type</em> button to find a card type.',
        '#attributes' => array('class' => array('column')),
    );

    $form['section']['cardtype']['card_type'] = array(
        '#type' => 'textfield',
        '#title' => t('Card Type'),
        '#size' => '30',
    );


    $form['section']['cardtype']['description'] = array(
        '#type' => 'textfield',
        '#title' => 'Description',
    );

    $form['section']['cardtype']['cardtype_search'] = array(
        '#type' => 'submit',
        '#value' => 'Search Card Type',
    );
 
    $form['section']['usable'] = array(
        '#type' => 'fieldset',
        '#title' => t('Usable'),
//        '#collapsible'=> TRUE,
//        '#collapsed' => FALSE,
        '#description' => 'Select the <em>Usable</em> e.',
        '#attributes' => array('class' => array('column')),
  );

  
    if ($arg) {
        $form['section']['cardtype']['card_type']['#default_value'] = $arg['card_type'];    
        $form['section']['cardtype']['description']['#default_value'] = $arg['description'];    
        $form['section']['product']['product_id']['#default_value'] = $arg['product_id'];    
        $form['section']['product']['product_name']['#default_value'] = $arg['product_name'];    
    } 
    
    return $form;
}

function ocs_admin_productcard_wizard_submit( $form, &$form_state)
{
    $values = $form_state['values'];
    if ( $values['op'] == 'Search Card Type') {
        $query = array(
            'card_type' => $values['card_type'],
            'description' => $values['description'],
        );
        drupal_goto( 'ocs_admin/card/cardtype/select', array( 'query' => $query));
    }
    else if ( $values['op'] == 'Search Product') {
        $query = array(
            'product_id' => $values['product_id'],
            'product_name' => $values['product_name'],
        );
        drupal_goto( 'ocs_admin/card/cardtype/product/select', array( 'query' => $query));
    }
   else {
        $form_state['redirect'] = 'ocs_admin/account';
        dpm( $form_state, 'submit: ');

        if ( $values['op'] == 'Confirm') {
            // save to DB
            dsm( 'confirmed: ');
            if ( ocs_admin_account_add( $values)) {
                drupal_set_message( 'A new account is created.');
            }
        }
    }
}


