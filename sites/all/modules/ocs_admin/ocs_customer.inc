<?php


/*
** add/edit members to group form
*/
function ocs_admin_customer_form( $form, &$form_state, $records = NULL)
{
    dpm( $records, 'members');

    /*
    foreach( $records as $record) {
        $key = $record['account_key'];
        $form['extension'][$key] = array(
            '#type' => 'textfield',
            '#default_value' => '000',
            '#size' => '50'
        );

        $records[$key]['extension'] = array(
            'data' => drupal_render( $form['extension'][$key]),
        );
    }
    */

    $header = array( 'customer_id' => 'Customer ID', 'customer_name' => 'Cutomer Name');
    $form['member'] =  array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $records,
        '#empty' => t('No content available.'),
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Confirm'),
    );

    $form['actions']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
    );

    return $form;
}

function ocs_admin_customer_form_submit( $form, &$form_state)
{
    $form_state['redirect'] = 'ocs_admin/group/' . $form_state['values']['ancestor_group_id'] . '/member';

    if ( $form_state['values']['op'] == 'Confirm') {
//        dpm( $form_state, 'submit group member');

        $member = array(
            'group_id' => $form_state['values']['group_id'],
            'ancestor_group_id' => $form_state['values']['ancestor_group_id'],
        );
        foreach( $form_state['values']['member'] as $key => $value) {
            if ( $value ) {
                // it is selected value
                $member['account_key'] = $value;
                ocs_admin_member_save( $member);
            }
        }

        drupal_set_message( t('Your configuration has been saved.'));
    }
}


/*
    add members
*/
function ocs_admin_customer_select( $arg = NULL)
{
    if ( $arg ) {
        $_SESSION['ocs_admin_customer_select'] = $arg;
    }
    else {
        $_SESSION['ocs_admin_customer_select'] = array();
    }

    $form = drupal_get_form( 'ocs_admin_customer_form', $_SESSION['ocs_admin_customer_select']);
    $output = '<div id="ocs_admin_customer_select_form_div">' . drupal_render( $form) . '</div>';

    return $output;
}

/*
**  ajax select button in account & product table
    impl: _panel, _popup, and etc
*/
function ocs_customer_select( $js = NULL, $type = 'customer', $id = NULL, $name = NULL)
{
    $arg = array( 
        'customer_id' => $id, 
        'customer_name' => $name,
    ); 

    /*
    $output = ocs_admin_customer_select( $arg);

    if ( $js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace( '#ocs_admin_customer_select_form_div', $output);
        print ajax_render( $commands);
        exit;
    }
    else {
        return $output;
    }
    */
    drupal_goto('ocs_admin/customer');
}


