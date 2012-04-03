<?php

/*
** generate a form to add a new account
*/
/*
function ocs_account_add_form( $form, &$form_state)
{
    $form['customer'] = array(
        '#type' => 'fieldset',
        '#title' => t('Customer'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
    );

    $form['customer']['id'] = array(
        '#type' => 'markup',
        '#markup' => t('Customer ID : @id', array( '@id' => 'a')),
    );

    $form['customer']['name'] = array(
        '#type' => 'markup',
        '#markup' => t('Customer Name : @name', array( '@name' => 'unknown')),
    );

    $form['customer']['block'] = array(
        '#type' => 'markup',
        '#markup' => t('block'),
        '#prefix' => '<div id="replace_block">',
        '#suffix' => '</div>',
    );
    $block = module_invoke('views', 'block_view', 'ocs_product-block');
    $view =  $block['content']['#views_contextual_links_info']['views_ui']['view'];
    $view->exposed_raw_input['product_name'] = 'sim';
    $form['view'] = $block['content'];

    $form['submit'] = array(
        '#type' => 'button',
        '#value' => 'Submit',
    );

    return $form;
}
*/

function ocs_account_add_form_submit( $form, &$form_state)
{
    $form_state['rebuild'] = TRUE;
}

/*
** generate form for checkbox
*/
function ocs_account_select_row( $args)
{
    ctools_include('ajax'); // Module  include the dependence it needs for ajax.

    $link = ctools_ajax_text_button( t('select'), "account/nojs/select/".$args, t('select'));
    return $link;
}

/*
** generate form for select
*/
/*
function ocs_account_select_row( $form, &$form_state)
{
    $form['select'] = array(
        '#type' => 'button',
        '#value' => 'Select',
        '#ajax' => array(
            'callback' => 'ocs_account_form_select_callback',
            'progress' => array(
                'message' => NULL,
            ),
        ),
    );

    return drupal_render( $form);
}
*/

function ocs_account_form_select_callback( $form, $form_state)
{
    return $form['select'];
}
