<?php

/*
** add/edit card type information form
*/
function ocs_admin_card_form( $form, &$form_state, $type, $cardtype)
{
//    dpm( $cardtype, 'Card Type');

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

function ocs_admin_product_card($form, &$form_state, $type, $cardtype) 
{

    $form['card_type'] = array(
        '#type' => 'hidden',
        '#value' => ($type == 'edit') ? $cardtype['card_type'] : NULL,
    );

		return $form;
}

function ocs_admin_card_form_submit( $form, &$form_state)
{
    $cardtype = $form_state['values'];

    if (ocs_admin_card_form_save($cardtype) != 0 ) {
	    drupal_set_message( t('Your configuration has been saved.'));
	    $form_state['redirect'] = 'ocs-card-type';
    } else {
  
 	    $form['unit'] = array('#type' => 'textfield', '#value' => $cardtype['unit']);
	    $form['active_period'] = array('#value' => $cardtype['active_period']);
	    $form['grace_period'] = array('#value' => $cardtype['grace_period']);
	    $form['description'] = array('#value' => $cardtype['description']);
    }

}
