<?php

/*
** add/edit card type information form
*/
function ocs_admin_card_form( $form, &$form_state, $type, $cardtype)
{    
//    dpm($type, 'ocs_card_type');

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

    if ( ! is_numeric( $cardtype['grace_peroid'])) {
         form_set_error('grace period', t('You have to enter numeric values the grace period.'));
    }
}


function ocs_admin_card_form_submin( $form, &$form_state )
{
    $cardtype = $form_state['values'];

    if (ocs_admin_card_form_save($cardtype) != 0 ) {
	    drupal_set_message( t('Your configuration has been saved.'));
	    $form_state['redirect'] = '/cardtype/cardtype';
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

function ocs_admin_product_card_add( $form, &$form_state, $type, $product_card)
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
