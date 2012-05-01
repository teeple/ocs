<?php

/*
** group over view page
** NOT USED. use views for this operation
*/
function ocs_admin_group_overview() 
{
    $result = array(
        array( 'title' => 'group',
            'group_name' => 'test group',
            'description' => 'test group',
            'group_id' => '1',
            ),
        array( 'title' => 'group',
            'group_name' => 'test group',
            'description' => 'test group',
            'group_id' => '1',
            ),
        array( 'title' => 'group',
            'group_name' => 'test group',
            'description' => 'test group',
            'group_id' => '1',
            ),
    );

    $header = array( t('Group'), array('data' => t('Operations'), 'colspan' => '3'));
    $rows = array();
    foreach( $result as $item) {
        $row = array( theme( 'ocs_admin_group_overview', 
            array( 'title' => $item['title'],
                'name' => $item['group_name'],
                'description' => $item['description'])));
        $row[] = array( 'data' => l(t('list members'), 'ocs_admin/group/'. $item['group_id']));
        $row[] = array( 'data' => l(t('list members'), 'ocs_admin/group/'. $item['group_id']));
        $row[] = array( 'data' => l(t('list members'), 'ocs_admin/group/'. $item['group_id']));
        $rows[] = $row;
    }

    return theme( 'table', array( 'header' => $header, 'rows' => $rows));
}

function theme_ocs_admin_group_overview( $variables) 
{
    $output = check_plain( $variables['title']);
    $output .= '<div class="description">' . filter_xss_admin($variables['description']). '</div>';

    return $output;
}

/*
** add/edit group information form
*/
function ocs_admin_group_edit_form( $form, &$form_state, $type, $groupid = NULL)
{
    $form = array();
    
    $form['section'] = array(
        '#type' => 'fieldset',
        '#description' => 'Input or select parameters to create an group.',
    );
    
    $form['section']['group'] = array(
        '#type' => 'fieldset',
        '#title' => t('Group'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => 'Click the <em>Search</em> button to find a account.',
        '#attributes' => array( 'class' => array( 'column')),
    );

    $form['section']['group']['group_id'] = array(
        '#type' => 'hidden',
        '#value' => NULL,
    );

    $form['section']['group']['parent_group_id'] = array(
        '#type' => 'hidden',
        '#value' => 0,
    );

    $form['section']['group']['ancestor_group_id'] = array(
        '#type' => 'hidden',
        '#value' => 0,
    );

    $form['section']['group']['group_name'] = array(
        '#type' => 'textfield',
        '#title' => 'Name',
        '#description' => t('Enter Group Name'),
        '#maxlength' => 64,
        '#size' => 30,
    );

    $form['section']['group']['master_number'] = array(
        '#type' => 'textfield',
        '#title' => 'Master Number',
        '#description' => t('Enter or select a master number to be used'),
        '#maxlength' => 24,
        '#size' => 30,
    );
    
    $form['section']['group']['account_search'] = array(
        '#type' => 'submit',
        '#value' => 'Search Account',
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

    if ( isset( $groupid)) {
        dpm( $groupid, 'group id');
        $group = group_load( $groupid);
        $form['section']['group']['group_id']['#value'] = $group['group_id'];
        $form['section']['group']['ancestor_group_id']['#value'] = $group['ancestor_group_id'];
        $form['section']['group']['parent_group_id']['#value'] = $group['parent_group_id'];
        $form['section']['group']['group_name']['#default_value'] = $group['group_name'];
        $form['section']['group']['master_number']['#default_value'] = $group['master_number'];
    }
    
    return $form;
}

function ocs_admin_group_edit_form_validate($form, &$form_state)
{
    $master_number = $form_state['values']['master_number'];

    // check master number another group 
    $group_id = ocs_admin_master_number_load($master_number);

    if (!is_null($group_id)) {
        form_set_error( 'master number', 
                        'Master number already exsits another group [group id:'.$group_id.']');
    }
}

function ocs_admin_group_edit_form_submit( $form, &$form_state)
{
    $group = $form_state['values'];
	  
    if ( $form_state['values']['op'] == 'Search Account') {
        $query = array(
            'account_key' => $form_state['values']['master_number'],
        );
        drupal_goto( 'ocs_admin/account/master/select', array( 'query' => $query));
    }
    else if ( $form_state['values']['op'] == 'Save') {
        dpm( $group, 'submit group');
        // check the master number other group
        
        ocs_admin_group_save($group);

        drupal_set_message( t('Your configuration has been saved.'));

        if ( $group['parent_group_id'] == 0)
            $form_state['redirect'] = 'ocs_admin/group';    // master group
        else    // subgroup
            $form_state['redirect'] = 'ocs_admin/group/' . $group['ancestor_group_id'] . '/subgroup';
    }
}

function ocs_admin_group_name_load($group_name)
{
    db_set_active('ocsdb');

    $result = db_select('ocs_group', 'g')
        ->fields('g', array('group_name'))
        ->condition('group_name', $group_name, '=')
        ->execute()
        ->fetchAssoc();

    return $result['group_name'];
}

function ocs_admin_master_number_load($master_number)
{
    db_set_active('ocsdb');

    $result = db_select('ocs_group', 'g')
        ->fields('g', array('ancestor_group_id'))
        ->condition('master_number', $master_number, '=')
        ->execute()
        ->fetchAssoc();

    return $result['ancestor_group_id'];
}

/*
** add/edit sub group information form
*/
function ocs_admin_subgroup_add_form( $form, &$form_state, $groupid)
{
    $group = group_load( $groupid);

    // list all subgroups belongs to this group
    $subgroups = ocs_admin_subgroup_load( $group['group_id']);

    $form['ancestor_group_id'] = array(
        '#type' => 'hidden',
        '#value' => $group['group_id'],
    );

    $options = array();
    foreach( $subgroups as $key => $value) {
        $options[ $key] = $value['group_name'];
    }

    $form['parent_group_id'] = array(
        '#type' => 'select',
        '#title' => 'Parent Group',
        '#options' => $options,
    );

    $form['group_name'] = array(
        '#type' => 'textfield',
        '#title' => 'Name',
        '#default_value' => $group['group_name'],
    );

    $form['master_number'] = array(
        '#type' => 'textfield',
        '#title' => 'Master Number',
        '#default_value' => $group['master_number'],
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

function ocs_admin_subgroup_add_form_submit( $form, &$form_state)
{
dpm($form, 'form');
    if ( $form_state['values']['op'] == 'Search Account') {
        $query = array(
            'account_key' => $values['master_number'],
        );
        drupal_goto( 'ocs_admin/account/master/select', array( 'query' => $query));
    }
    else if ( $form_state['values']['op'] != 'Cancel') {
        $group = $form_state['values'];
        dpm( $group, 'submit subgroup');
        ocs_admin_group_save($group);

        drupal_set_message( t('Your configuration has been saved.'));
    }

    $form_state['redirect'] = 'ocs_admin/group/' . $group['ancestor_group_id'] . '/subgroup';
}


/*
    get group information
*/
function ocs_admin_db_get_group( $groupid)
{
    db_set_active('ocsdb');

    // get group information
    $query = db_select( 'ocs_group', 'g');
    $result = $query->condition( 'g.group_id', $groupid, '=')
        ->fields( 'g')
        ->execute()
        ->fetchAllAssoc( 'group_id', PDO::FETCH_ASSOC);

    db_set_active('default');

    return $result;
}

/*
    get subgroup list
*/
function ocs_admin_subgroup_load( $groupid)
{
    db_set_active('ocsdb');

    // get group information
    $result = db_select( 'ocs_group', 'g')
        ->condition( 'g.ancestor_group_id', $groupid, '=')
        ->fields( 'g')
        ->execute()
        ->fetchAllAssoc( 'group_id', PDO::FETCH_ASSOC);

    db_set_active( 'default');

    return $result;
}

/* 
    get group id [param:master number]
*/
function ocs_admin_db_get_group_count( $group_id, $master_number)
{
    db_set_active('ocsdb');

    // get group information
    $query = db_select( 'ocs_group', 'g');
    $result = $query->condition( 'g.master_number', $master_number, '=')
        ->condition('g.ancestor_group_id', $group_id, '!=')
        ->fields( 'g')
        ->execute()
        ->fetchAllAssoc( 'group_id', PDO::FETCH_ASSOC);
    
    db_set_active('default');

    return $result;
}



/*
** group form
*/
function ocs_admin_group_form( $form, &$form_state, $groupid)
{
    $group = group_load( $groupid);
//  dpm( $group, 'Admin group form');

  $form['#attached']['css'] = array(drupal_get_path('module', 'ocs_admin') . '/ocs_admin.css');

  $form['#description'] = '<p>Change the hierarchy of subgroups by moving records.<br>'
        .'Click <em>Edit</em> button to change its properties.<br>'
        .'Click <em>Delete</em> button to remove the subgroup.</p>';

  // get all subgroups
  $subgroups = ocs_admin_subgroup_load( $group['group_id']);
  $tree = ocs_admin_build_tree( $subgroups, array( 
    'title' => 'group_name',
    'plid' => 'parent_group_id', 
    'mlid' => 'group_id'), -1);

//  dpm( $subgroups, 'subgroup');
  $tree = ocs_admin_get_sub_tree( array_shift( $tree));
 // dpm( $tree, 'tree');

  $form = array_merge($form, _ocs_group_tree_form($tree));

  if (element_children($form)) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    );
    $form['actions']['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
    );
  }
  else {
    $form['#empty_text'] = t('There are no subgroups yet. <a href="@link">Add Subgroup</a>.', 
        array('@link' => url('ocs_admin/group/'. $group['group_id'] .'/add')));
  }

  return $form;
}

/*
    return the fist level subtree
*/
function ocs_admin_get_sub_tree( $tree) 
{
    foreach( $tree['below'] as $node) {
        $node['plid'] = 0;
    }

    return $tree['below'];
}



/**
 * Recursive helper function for ocs_group_form().
 *
 * @param $tree
 *   The menu_tree retrieved by menu_tree_data.
 */
function _ocs_group_tree_form($tree) {
  $form = &drupal_static(__FUNCTION__, array('#tree' => TRUE));
  foreach ($tree as $data) {
    $title = '';
    $item = $data;
    if ($item ) {
      $mlid = 'mlid:' . $item['mlid'];
      $form[$mlid]['#item'] = $item;
      $form[$mlid]['#attributes'] = array( 'class' => array('menu-enabled'));
      $form[$mlid]['title']['#markup'] = $item['title'];
      $form[$mlid]['weight'] = array(
        '#type' => 'weight',
        '#delta' => 50,
//        '#default_value' => $item['weight'],
        '#default_value' => 50,
        '#title_display' => 'invisible',
        '#title' => t('Weight for @title', array('@title' => $item['title'])),
      );
       $form[$mlid]['mlid'] = array(
        '#type' => 'hidden',
        '#value' => $item['mlid'],
      );
      $form[$mlid]['plid'] = array(
        '#type' => 'hidden',
        '#default_value' => $item['plid'],
      );
      // Build a list of operations
      $operations = array();
      $operations['edit'] = array( 
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => 'ocs_admin/group/'. $item['mlid'] . '/edit');
      $operations['delete'] = array( 
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => 'ocs_admin/group/'. $item['mlid'] . '/delete');
      $form[$mlid]['operations'] = $operations;  
    }

    if ( isset($data['below'])) {
      _ocs_group_tree_form($data['below']);
    }
  }
  return $form;
}

function ocs_admin_group_form_submit( $form, &$form_state)
{
//    dpm( $form_state, 'form_state');

    if ( $form_state['values']['op'] != 'Cancel') {
        $order = array_flip( array_keys( $form_state['input']));
        $form = array_merge( $order, $form);

    //    dpm( $order, 'order');
     //   dpm( $form, 'form');

        $updated_items = array();
        $fields = array( 'plid' => 'parent_group_id');

        foreach( element_children( $form) as $mlid) {
            if ( isset( $form[$mlid]['#item'])) {
                $element = $form[$mlid];
                // update any fields that have changed in this menu item.
                foreach( $fields as $field => $db_field) {
                    if ( $element[$field]['#value'] != $element[$field]['#default_value']) {
                        $element['#item']['db'][$db_field] = $element[$field]['#value'];
                        $updated_items[$mlid] = $element['#item'];
    //                    dpm( $updated_items[$mlid], 'updated');
                    }
                }
                // hidden is a special case, the value needs to be reserved
                /*
                if ( $element['hidden']['#value'] != $element['hidden']['#default_value']) {
                    // convert to integer rather than boolean due to PDO cast to string
                    $element['#item']['hidden'] = $element['hidden']['#value'] ? 0 : 1;
                    $updated_items[$mlid] = $element['#item'];
                    dpm( $updated_items[$mlid], 'updated hidden');
                }
                */
            }
        }

        foreach( $updated_items as $item) {
            ocs_admin_group_save( $item['db']);
        }

        drupal_set_message( t('Your configuration has been saved.'));
    }

//    $form_state['redirect'] = 'ocs_admin/group/';
}

function ocs_admin_group_title( $op, $group)
{
    return t( $op. ' @sub "@name"', 
        array( '@sub' => ( $group['parent_group_id'] == 0) ? 'Group' : 'Sub Group',
            '@name' => $group['group_name']));
}


/**
 * Returns HTML for the menu overview form into a table.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_ocs_admin_group_form($variables, $header = NULL) 
{
  
  $form = $variables['form'];

  if ( ! isset( $header)) {
      $header = array(
        t('Group Name'),
        t('Weight'),
        array( 'data' => t('Operations'), 'colspan' => '3'),
      );
  }

  return ocs_admin_tree_table_theme( $form, $header);
}


/*
** delete subgroup
*/
function ocs_admin_group_delete_confirm( $form, &$form_state, $group)
{
    $form['#group'] = $group;

    if ( $group['parent_group_id'] == 0) {
        // delete the whole group
        $num_members = ocs_admin_group_members_count( $group['group_id'], 'ancestor_group_id');
        $redirect = 'ocs_admin/group/';
        $msg = t('Are you sure to delete the whole group "%name"?', array( '%name' => $group['group_name']));
    }
    else{
        // delete subgroup
        $num_members = ocs_admin_group_members_count( $group['group_id'], 'group_id');
        $redirect = 'ocs_admin/group/' . $group['ancestor_group_id'];
        $msg = t('Are you sure to delete the subgroup "%name"?', array( '%name' => $group['group_name']));
    }

    $caption = '';
    if ( $num_members) {
        $caption .= '<p>' . format_plural( $num_members, 
            '<strong>Warning:</strong> There is currently 1 member in "%name". It will be deleted.', 
            '<strong>Warning:</strong> There are currently @count members in "%name". They will be deleted.', 
            array( '%name' => $group['group_name'])) . '</p>';
    }

    $caption .= '<p>'. t('This action cannot be undone.') . '</p>';

    return confirm_form( $form, $msg, $redirect, $caption, t('Delete'));
}

function ocs_admin_group_members_count( $group_id, $key)
{
    db_set_active('ocsdb');

    $query = db_select( 'ocs_group_member', 'g')
        ->fields( 'g', array( 'account_key'))
        ->condition( $key, $group_id, '=');

    $query->addExpression( 'COUNT(account_key)', 'member_count');

    $result = $query->execute()->fetchAssoc();
    //dpm( $result, 'fetched group member count');

    db_set_active('default');

    return $result['member_count'];
}

function ocs_admin_group_delete_confirm_submit( $form, &$form_state)
{
    $group = $form['#group'];

    if ( $group['parent_group_id'] == 0)
        ocs_admin_group_delete( $group);
    else
        ocs_admin_subgroup_delete( $group);

    $t_args = array( '%name' => $group['group_name']);
    drupal_set_message( t('The group %name has been deleted.', $t_args));
    watchdog( 'ocs_admin', 'Deleted group %name.', $t_args, WATCHDOG_NOTICE);

    if ( $group['parent_group_id'] == 0)
        $form_state['redirect'] = 'ocs_admin/group/';
    else
        $form_state['redirect'] = 'ocs_admin/group/'. $group['ancestor_group_id'];
}

/*
** add/edit members to group form
*/
function ocs_admin_group_member_form( $form, &$form_state, $groupid = NULL, $records = NULL)
{

    $group = group_load( $groupid);
    dpm( $group, 'group');

    $form['group_id'] =  array(
        '#type' => 'hidden',
        '#value' => $group['group_id'],
    );

    $form['ancestor_group_id'] =  array(
        '#type' => 'hidden',
        '#value' => $group['ancestor_group_id'],
    );

    $header = array( 'account_key' => 'Account Key', 'customer_name' => 'Cutomer Name');
    $form['member'] =  array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $records,
        '#empty' => t('No content available.'),
    );

    $keys = array_keys( $records);
    // build default key
    if ( count( $keys) > 0) {
        $form['member']['#default_value'] = array_combine( $keys, array_pad( array(1), count( $keys), 1));
    }

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

function ocs_admin_group_member_form_submit( $form, &$form_state)
{
    $form_state['redirect'] = 'ocs_admin/group/' . $form_state['values']['ancestor_group_id'] . '/member';

    dpm( $form_state, 'submit group member');
    if ( $form_state['values']['op'] == 'Confirm') {

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
function ocs_admin_member_add( $groupid = NULL, $arg = NULL)
{
    if ( $arg ) {
        $_SESSION['ocs_admin_member_add'][ $arg['account_key'] ] = $arg;
    }
    else {
        $_SESSION['ocs_admin_member_add'] = array();
    }

    $form = drupal_get_form( 'ocs_admin_group_member_form', $groupid, $_SESSION['ocs_admin_member_add']);
    $output = '<div id="ocs_admin_member_add_form_div">' . drupal_render( $form) . '</div>';

    return $output;
}

/*
**  ajax select button in account & product table
*/
function ocs_member_select( $groupid, $js = NULL)
{

    $arg = array( 
        'account_key' => $_GET['id'], 
        'customer_name' => $_GET['name'],
        $_GET['id'] => $_GET['id'],
    ); 

    dpm( $arg, 'arg');
    $output = ocs_admin_member_add( $groupid, $arg);

    if ( $js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace( '#ocs_admin_member_add_form_div', $output);
        print ajax_render( $commands);
        exit;
    }
    else {
        return $output;
    }
}

/*
** edit group information form
*/
function ocs_admin_member_edit_form( $form, &$form_state, $group, $account_key)
{
    $form['group_id'] = array(
        '#type' => 'hidden',
        '#value' => $group['group_id'],
    );

    $form['account_key'] = array(
        '#type' => 'hidden',
        '#value' => $account_key,
    );

    $account = ocs_admin_member_load( $account_key);

    $form['extension'] = array(
        '#type' => 'textfield',
        '#title' => 'Extension',
        '#description' => 'Extension number in VPN group. (3~4 digits)',
        '#default_value' => $account['extension'],
    );

    $form['limitation'] = array(
        '#type' => 'textfield',
        '#title' => 'Limitation',
        '#description' => 'Usage Limitation of this member',
        '#default_value' => $account['limitation'],
    );

    $form['account_usage'] = array(
        '#type' => 'textfield',
        '#title' => 'Usage',
        '#description' => 'Current Usage of this member',
        '#default_value' => $account['account_usage'],
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

function ocs_admin_member_edit_form_submit( $form, &$form_state)
{
    $member = $form_state['values'];
    if ( $form_state['values']['op'] == 'Save') {
        dpm( $member, 'update member');
        ocs_admin_member_save( $member);
        drupal_set_message( t('The information has been updated.'));
    }

    $form_state['redirect'] = 'ocs_admin/group/' . $member['group_id'] . '/member/';
}

/*
** delete subgroup
*/
function ocs_admin_member_delete_confirm( $form, &$form_state, $groupid, $account_key)
{
    $group = group_load( $groupid);
    $form['#group'] = $group;
    $form['#account_key'] = $account_key;
    $caption = '<p>' . 
            t('<strong>Warning:</strong> The member account "%key" will be deleted from the group "%group".',
                array( '%key' => $account_key, '%group' => $group['group_name'])) .
            '</p>';

    $caption .= '<p>'. t('This action cannot be undone.') . '</p>';

    return confirm_form( $form, t('Are you sure to delete the member "%key"?',
        array( '%key' => $account_key)),
        'ocs_admin/group/' . $group['group_id'] . '/member',
        $caption, t('Delete'));
}

function ocs_admin_member_delete_confirm_submit( $form, &$form_state)
{
    $group = $form['#group'];
    $account_key = $form['#account_key'];
    ocs_admin_member_delete( $account_key);

    $t_args = array( '%key' => $account_key);
    drupal_set_message( t('The member %key has been deleted.', $t_args));
    watchdog( 'ocs_admin', 'Deleted group member %key.', $t_args, WATCHDOG_NOTICE);

    $form_state['redirect'] = 'ocs_admin/group/'. $group['group_id'] . '/member';
}



/*
    save OCS Customer Group
*/
function ocs_admin_group_save( $group)
{
    db_set_active( 'ocsdb');

    $fields = array( 
        'group_id' => isset($group['group_id']) ? $group['group_id'] : NULL,
        'ancestor_group_id' => isset($group['ancestor_group_id']) ? $group['ancestor_group_id'] : 0,
        'parent_group_id' => isset($group['parent_group_id']) ? $group['parent_group_id'] : 0,
        'group_name' => isset($group['group_name']) ? $group['group_name'] : NULL,
        'service_type' => isset($group['service_type']) ? $group['service_type'] : 0,
        'group_type' => isset($group['group_type']) ? $group['group_type'] : 0,
        'member_count' => isset($group['member_count']) ? $group['member_count'] : 0,
        );
    if ( isset( $group['master_number']))
        $fields['master_number'] = $group['master_number'];

    if ( isset( $group['group_id'])) {
        $status = db_merge( 'ocs_group')
            ->key( array( 'group_id' => $group['group_id']))
            ->fields( $fields)
            ->execute();
        dpm( $status, 'db_merge result');
    }
    else {
        // insert
        $options = array('return' => Database::RETURN_INSERT_ID);
        $last_insert_id = db_insert( 'ocs_group', $options)
            ->fields( $fields)
            ->execute();
        dpm( $last_insert_id, 'db_insert result');

        // update ancestor_group_id
        if ( $fields['ancestor_group_id'] == 0 && $last_insert_id > 0) {
            $result = db_update( 'ocs_group')
                ->fields( array(
                    'ancestor_group_id' => $last_insert_id
                ))
                ->condition( 'group_id', $last_insert_id, '=')
                ->execute();
            dpm( $result, 'db_update result');
        }
    }

    db_set_active( 'default');
}

function ocs_admin_group_delete( $group)
{
    db_set_active( 'ocsdb');

    // delete the group and all subgroups
    $status = db_delete( 'ocs_group')
        ->condition( 'ancestor_group_id', $group['group_id'])
        ->execute();
    dpm( $status, 'db_delete result');

    db_set_active( 'default');
}

function ocs_admin_subgroup_delete( $group)
{
    db_set_active( 'ocsdb');

    // delete
    $status = db_delete( 'ocs_group')
        ->condition( 'group_id', $group['group_id'])
        ->execute();
    dpm( $status, 'db_delete result');

    // update
    $status = db_update( 'ocs_group')
        ->fields( array(
            'parent_group_id' => $group['parent_group_id']
            ))
        ->condition( 'ancestor_group_id', $group['ancestor_group_id'], '=')
        ->condition( 'parent_group_id', $group['group_id'], '=')
        ->execute();
    dpm( $status, 'db_update result');

    db_set_active( 'default');
}

/*
    save OCS group member 
*/
function ocs_admin_member_save( $member)
{
    db_set_active( 'ocsdb');

    $fields = array();
    foreach( array( 'group_id', 'ancestor_group_id', 'account_key', 'extension', 'account_usage') as $field) {
        if ( isset( $member[$field]))
            $fields[ $field] = $member[ $field];
    }

    dpm( $fields, 'member save');

     // insert
     $status = db_merge( 'ocs_group_member')
        ->key( array( 'account_key' => $member['account_key']))
        ->fields( $fields)
        ->execute();
    dpm( $status, 'db_merge result');

    db_set_active( 'default');
}


/*
    load member info
*/
function ocs_admin_member_load( $account_key)
{
   db_set_active('ocsdb');

   $query = db_select( 'ocs_group_member', 'g');
   $result = $query->fields( 'g')
        ->condition( 'account_key', $account_key, '=')
        ->execute();

    $member = $result->fetchAssoc();
    dpm( $member, 'fetched group member');

    db_set_active('default');

    return $member;
}

/*
    load account keyr info
*/
function ocs_admin_account_load( $master_number)
{
   db_set_active('ocsdb');

   $query = db_select( 'ocs_account', 'g');
   $result = $query->fields( 'g')
        ->condition( 'account_key', $master_number, '=')
        ->condition( 'status', '1', '=')
        ->execute();

    $member = $result->fetchAssoc();

    db_set_active('default');

    return $member;
}


/*
    load member info
*/
function ocs_admin_group_member_load( $group_id, $fields = array())
{
   db_set_active('ocsdb');

   // add 'account_key' if $fields is passed
   if ( count( $fields) > 0 && !array_key_exists( 'account_key', $fields)) {
       $fields[] = 'account_key';
   }

   $query = db_select( 'ocs_group_member', 'g');
   $result = $query->fields( 'g', $fields)
        ->orderBy( 'group_id')
        ->condition( 'ancestor_group_id', $group_id, '=')
        ->execute()
        ->fetchAllAssoc( 'account_key', PDO::FETCH_ASSOC);

    db_set_active('default');

    return $result;
}


/*
    delete member info
*/
function ocs_admin_member_delete( $account_key)
{
   db_set_active('ocsdb');

   $result = db_delete( 'ocs_group_member')
        ->condition( 'account_key', $account_key, '=')
        ->execute();

    dpm( $result, 'db_delete group member');

    db_set_active('default');

    return $result;
}


function cmp_group_id( $a, $b)
{
    return $a['group_id'] - $b['group_id'];
}

function ocs_admin_member_list( $form, &$form_state, $groupid)
{
    $group = group_load( $groupid);

    $form['group_id'] = array(
        '#type' => 'hidden',
        '#value' => $groupid,
    );

    $members = ocs_admin_group_member_load( $group['group_id']);
    $subgroup = ocs_admin_subgroup_load( $group['group_id']);
    $form['#subgroup'] = $subgroup;

    // build subgroup select options
    $options = array();
    foreach( $subgroup as $item) {
        $options[ $item['group_id']] = $item['group_name'];
    }

    $form['members'] = array(
        '#type' => 'container',
        '#prefix' => '<div id="member_group_div">',
        '#suffix' => '</div>',
    );

    foreach( $members as $key => $member) {
//        dpm( $member, 'member');

        $sub = &$form['#subgroup'][ $member['group_id']];
        $sub['members'][] = $key;

        $form['members'][$key][$key.':ancestor_group_id'] = array(
            '#type' => 'value',
            '#value' => $member['ancestor_group_id'],
            '#default_value' => $member['ancestor_group_id'],
        );

        $form['members'][$key][$key.':account_key'] = array(
            '#type' => 'item',
            '#value' => $member['account_key'],
            '#default_value' => $member['account_key'],
        );

        $form['members'][$key][$key.':group_id'] = array(
            '#type' => 'select',
            '#options' => $options,
            '#default_value' => $member['group_id'],
            /*
            '#ajax' => array(
                'callback' => 'ocs_admin_change_subgroup_js',
                'wrapper' => 'member_group_div',
            ),
            */
        );

        $form['members'][$key][$key.':account_usage'] = array(
            '#type' => 'textfield',
            '#size' => 10,
            '#title' => '',
            '#default_value' => $member['account_usage'],
        );

        $form['members'][$key][$key.':extension'] = array(
            '#type' => 'textfield',
            '#size' => 10,
            '#title' => '',
            '#default_value' => $member['extension'],
            '#maxlength' => 4,
        );

        $form['members'][$key]['#attributes'] = array( 'class' => array('menu-enabled'));

      // Build a list of operations
      $operations = array();
      $operations['remove'] = array( 
        '#type' => 'link',
        '#title' => t('remove'),
        '#href' => 'ocs_admin/group/'. $groupid . '/member/' . $member['account_key'] . '/delete');
      $form['members'][$key]['operations'] = $operations;  
    }

    $form['actions'] = array(
        '#type' => 'actions',
    );

    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save Configuration'),
    );
    $form['actions']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
    );
    $form['actions']['#attributes'] = array();

    return $form;
}


/*
function ocs_admin_change_subgroup_js( $form, $form_state)
{
}
*/


/**
 */
function theme_ocs_admin_member_list( $variables)
{
    dsm('member_list');
    drupal_add_tabledrag('menu-overview', 'order', 'sibling', 'menu-weight');
    $form = $variables['form'];
//    dpm( $form, 'theme');

    $rows = array();
    $groupid = NULL;
    $subgroup = $form['#subgroup'];

    foreach( $subgroup as $sub) {
//        dpm( $sub, 'sub');
        $rows[] =  array( array( 'data' => t( 'Members of Subgroup <em>@group</em>', array( '@group' => $sub['group_name'])), 'colspan' => 7));

        if ( isset( $sub['members'])) {
            foreach ($sub['members'] as $key ) {
                $elm = &$form['members'][$key];
//                dpm( $elm, 'elm');

                $row = array();
                $row[] = drupal_render($elm[$key.':group_id']);
                $row[] = $key;
                $row[] = drupal_render( $elm[$key.':account_usage']);
                $row[] = drupal_render( $elm[$key.':extension']);
                $row[] = drupal_render( $elm['operations']);

                $row = array_merge(array('data' => $row), $elm['#attributes']);
        //        $row['class'][] = 'draggable';
                $rows[] = $row;
            }
        }
        else {
            $rows[] =  array( array( 'data' => t( 'No members'), 'colspan' => 7));
        }
    }

//    dpm( $rows, 'row');

    if (empty($rows)) {
        $rows[] = array(array('data' => $form['#empty_text'], 'colspan' => '7'));
    }
    if ( isset($form['#footer'])) {
        $rows[] = array(array('data' => $form['#footer'], 'colspan' => '7'));
    }

    $output = '';
    if ( isset( $form['#caption'])) {
        $output .= '<h3>' . $form['#caption'] . '</h3>';
    }
    $header = array( t('Sub Group'), t('Account Key'), t('Usage'), t('Extension'), t('Operations'));
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'menu-overview')));
    $output .= drupal_render_children($form);

    return $output;
}

function ocs_admin_member_list_validate( $form, &$form_state)
{
    $values = $form_state['values'];
//    dpm( $form, 'form');
    
    if ( $values['op'] != 'Cancel') {
        // check extension
        $ext = ocs_admin_member_extension( $values['group_id']);    // get all extensions 
        foreach( $values as $key => $value) {
            $list = explode( ':', $key);
            if ( isset( $list[1])) {
                if ( $list[1] == 'extension' ) {
                    if ( ! is_numeric( $value) || strlen( $value) < 3) {
                        form_set_error( $key, 'Invalid extension. 3~4 digits are required.');
                    }
                    else if ( isset( $ext[ $value])) {
                        form_set_error( $key, 'Duplicated Extension.');
                        form_set_error( $ext[$value], 'Duplicated Extension.');
                    }
                    $ext[ $value] = $key;
                }
                else if ( $list[1] == 'usage' ) {
                    if ( ! is_numeric( $value)) {
                        form_set_error( $key, 'Invalid usage. Digits are required.');
                    }
                }
            }
        }
    }
}

// return all the members' extension
function ocs_admin_member_extension( $groupid)
{
    $members = ocs_admin_group_member_load( $groupid, array( 'extension'));

    $ext = array();
    foreach( $members as $member) {
        $ext[] = $member['extension'];
    }
    return $ext;
}

function ocs_admin_member_list_submit( $form, &$form_state)
{
    $values = $form_state['values'];
    
    if ( $values['op'] != 'Cancel') {
        // get updated values
        $update = array();
        foreach( $values as $key => $value) {
            $list = explode( ':', $key);
            if ( isset( $form['members'][$list[0]]) && 
                 (!isset( $form['members'][$list[0]][$key]['#default_value']) || 
//                 $list[1] == 'group_id' ||
                 $form['members'][$list[0] ][$key]['#default_value'] != $value)) {
                $updated[$list[0]][$list[1]] = $value;
            }
        }

        foreach( $updated as $key => $value) {
 //           if ( count( $value) > 1) {
                // as 'group_id' field is always included, the count must be bigger than 1
                // to save the changes
                dpm( $value, 'value');
                ocs_admin_member_save( array_merge( array('account_key' => $key), $value));
  //          }
        }
    }
}


/*
    load group 
*/
function group_load( $groupid) 
{
    $all_group = group_load_all();
    return isset( $all_group[$groupid]) ? $all_group[$groupid] : FALSE;
}

/*
    load all group
*/
function group_load_all()
{
   $groups = &drupal_static(__FUNCTION__);
   if ( !isset( $groups)) {
       db_set_active('ocsdb');

       $query = db_select( 'ocs_group', 'g');
       $groups = $query->fields( 'g')
            ->execute()
            ->fetchAllAssoc( 'group_id', PDO::FETCH_ASSOC);

//        dpm( $groups, 'fetched groups');

        db_set_active('default');
    } 

    return $groups;
}

/*
**  ajax select button in ocs_group table
*/
function ocs_admin_group_select($impl, $js=NULL, $type='group', $id=NULL)
{
    dsm('ocs_admin_group_select');
    
    $arg = array(
        $type . '_id' => $id,
    );
    
    $output = ocs_admin_group_add_page($impl, $arg);

    if ($js) {
        ctools_include('ajax');
        $commands = array();
        $commands[] = ajax_command_replace('#ocs_admin_group_page_div', $output);
        print ajax_render($commands);
        exit;
    }
    else {
        return $output;
    }
}

function ocs_admin_group_add_page( $impl = NULL, $arg = NULL, $init = TRUE)
{
    $impl = '_wizard';
    if (! isset($_SESSION['ocs_admin_group_save'])) {
        $_SESSION['ocs_admin_group_save'] = array();
    }
    $_SESSION['ocs_admin_group_save'] = array_merge($_SESSION['ocs_admin_group_save'], $_GET);
    
    $form = drupal_get_form('ocs_admin_group_add_page'.$impl, $_SESSION['ocs_admin_group_save']);
    $output = '<div id="ocs_admin_group_page_div">' . drupal_render( $form) . '</div>';
    
    return $output;
}

function ocs_admin_group_add_page_wizard( $form, &$form_state, $arg = NULL)
{
    $form = array();
    $form['section'] = array(
        '#type' => 'fieldset',
        '#description' => 'Input or select parameters to create an group.',
    );

    $form['section']['group'] = array(
        '#type' => 'fieldset',
        '#title' => t('Group'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => 'Click the <em>Search</em> button to find a account.',
        '#attributes' => array( 'class' => array( 'column')),
    );

    $form['section']['group']['group_id'] = array(
        '#type' => 'hidden',
        '#value' => NULL,
    );
    
    $form['section']['group']['group_type'] = array(
        '#type' => 'hidden',
        '#value' => NULL,
        '#default_value' => 0,
    );
 
    $form['section']['group']['member_count'] = array(
        '#type' => 'hidden',
        '#value' => NULL,
        '#default_value' => 0,
    );

    $form['section']['group']['parent_group_id'] = array(
        '#type' => 'hidden',
        '#value' => 0,
    );

    $form['section']['group']['ancestor_group_id'] = array(
        '#type' => 'hidden',
        '#value' => 0,
    );

    $form['section']['group']['group_name'] = array(
        '#type' => 'textfield',
        '#title' => 'Name',
        '#description' => t('Enter Group Name'),
        '#maxlength' => 64,
        '#size' => 30,
        '#required' => TRUE,
    );

    $form['section']['group']['master_number'] = array(
        '#type' => 'textfield',
        '#title' => 'Master Number',
        '#description' => t('Enter or select a master number to be used'),
        '#maxlength' => 24,
        '#size' => 30,
    );

    $form['section']['group']['account_search'] = array(
        '#type' => 'submit',
        '#value' => 'Search Account',
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

    if ( isset($arg['group_id'])) {
        $group = group_load( $arg['group_id']);
        $form['section']['group']['group_id']['#value'] = $group['group_id'];
        $form['section']['group']['ancestor_group_id']['#value'] = $group['ancestor_group_id'];
        $form['section']['group']['parent_group_id']['#value'] = $group['parent_group_id'];
        $form['section']['group']['group_name']['#default_value'] = $group['group_name'];
        $form['section']['group']['master_number']['#default_value'] = $group['master_number'];
    } else if (isset($arg['account_key']) && ($_SESSION['form_mode'] != 'I')) {
        $form['section']['group']['group_name']['#default_value'] = $_SESSION['group_name'];
        $form['section']['group']['master_number']['#default_value'] = $arg['account_key'];
        $_SESSION['group_name'] = NULL;
        $_SESSION['form_mode'] = 'I';
    }
    
    return $form;
}

function ocs_admin_group_add_page_wizard_submit( $form, &$form_state)
{
    $group = $form_state['values'];
    
    if ( $form_state['values']['op'] == 'Search Account') {
        $_SESSION['form_mode'] = 'M';
        $_SESSION['group_name'] = $group['group_name'];
        $query = array(
            'account_key' => $form_state['values']['master_number'],
        );

        drupal_goto( 'ocs_admin/account/master/select', array( 'query' => $query));
    }
    else if ( $form_state['values']['op'] == 'Save') {
        // check the master number other group
        
        ocs_admin_group_save($group);

        drupal_set_message( t('Your configuration has been saved.'));
        $_SESSION['form_mode'] = 'I';
        if ( $group['parent_group_id'] == 0)
            $form_state['redirect'] = 'ocs_admin/group';    // master group
        else    // subgroup
            $form_state['redirect'] = 'ocs_admin/group/' . $group['ancestor_group_id'] . '/subgroup';
    }
}

function ocs_admin_group_add_page_wizard_validate($form, &$form_state)
{
    if ($form_state['values']['op'] == 'Save') {  
        $master_number = $form_state['values']['master_number'];
   
        // check master number another group 
        $group_id = ocs_admin_master_number_load($master_number);
        if (!is_null($group_id)) {
            form_set_error( 'master number', 
                            'Master number already exsits another group [group id:'.$group_id.']');
        }

        $account_key = ocs_admin_account_load($master_number);
        dpm($account_key, 'account_key');
        if (is_null($account_key['account_key'])) {
            form_set_error( 'master number', 'Invalid master number');
        }

        $group_name = ocs_admin_group_name_load($form_state['values']['group_name']);
        dpm($group_name, 'group_name');
        if ($group_name == $form_state['values']['group_name']){
            form_set_error( 'group_name', 'Already exists group name');    
        }
    }
}


