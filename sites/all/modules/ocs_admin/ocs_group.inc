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
function ocs_admin_group_edit_form( $form, &$form_state, $type, $group)
{
    $form['group_id'] = array(
        '#type' => 'hidden',
        '#value' => ($type == 'edit') ? $group['group_id'] : NULL,
    );

    $form['parent_group_id'] = array(
        '#type' => 'hidden',
        '#value' => ($type == 'edit') ? $group['parent_group_id'] : 0,
    );

    $form['ancestor_group_id'] = array(
        '#type' => 'hidden',
        '#value' => ($type == 'edit') ? $group['ancestor_group_id'] : 0,
    );

    $form['group_name'] = array(
        '#type' => 'textfield',
        '#title' => 'Name',
        '#description' => t('Enter Group Name'),
        '#default_value' => ($type == 'edit') ? $group['group_name'] : NULL,
    );

    $form['master_number'] = array(
        '#type' => 'textfield',
        '#title' => 'Master Number',
        '#description' => t('Enter Master number to be used'),
        '#default_value' => ($type == 'edit') ? $group['master_number'] : NULL,
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

    return $form;
}

function ocs_admin_group_edit_form_submit( $form, &$form_state)
{
    $group = $form_state['values'];
    dpm( $group, 'submit group');
    ocs_admin_group_save($group);

    drupal_set_message( t('Your configuration has been saved.'));

    if ( $group['parent_group_id'] == 0)
        $form_state['redirect'] = 'ocs_admin/group';    // master group
    else    // subgroup
        $form_state['redirect'] = 'ocs_admin/group/' . $group['ancestor_group_id'];
}

/*
** add/edit sub group information form
*/
function ocs_admin_group_subgroup_form( $form, &$form_state, $type, $group)
{
    // list all subgroups belongs to this group
    $subgroups = ocs_admin_db_get_subgroup( $group['group_id']);

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

    return $form;
}

function ocs_admin_group_subgroup_form_submit( $form, &$form_state)
{
    $group = $form_state['values'];
    dpm( $group, 'submit subgroup');
    ocs_admin_group_save($group);

    drupal_set_message( t('Your configuration has been saved.'));

    $form_state['redirect'] = 'ocs_admin/group/' . $group['ancestor_group_id'];
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
function ocs_admin_db_get_subgroup( $groupid)
{
    db_set_active('ocsdb');

    // get group information
    $query = db_select( 'ocs_group', 'g');
    $result = $query->condition( 'g.ancestor_group_id', $groupid, '=')
        ->fields( 'g')
        ->execute()
        ->fetchAllAssoc( 'group_id', PDO::FETCH_ASSOC);

    db_set_active( 'default');

    return $result;
}

function _ocs_admin_build_path( $list, &$node)
{
    $parent = $node['parent_group_id'];
    $path = array( $node['group_id']);
    
    if ( $parent == '0') {
        return $path;
    }
    else {
        if ( isset($list[$parent])) {
            return array_merge( _ocs_admin_build_path( $list, $list[$parent]), $path);
        }
        else {
            // parent not found
            // reset parent group id
            $node['parent_group_id'] = 0;
            return $path;
        }
    }
}

function _ocs_admin_build_tree( &$tree, $path, $item)
{
    if ( count( $path) == 1) {
        $tree[$item['mlid']] = $item;   // root node
        return;
    }

    // remove the first node
    $elm = $path[0];
    unset( $path[0]);

    return _ocs_admin_build_tree( $tree[$elm]['below'], array_values( $path), $item);
}


/*
** group form
*/
function ocs_admin_group_form( $form, &$form_state, $group)
{
//  dpm( $group, 'Admin group form');

  $form['#attached']['css'] = array(drupal_get_path('module', 'ocs_admin') . '/ocs_admin.css');

  // get all subgroups
  $subgroups = ocs_admin_db_get_subgroup( $group['group_id']);

  // first calculate the depth
  $depthlist = array();
  foreach( $subgroups as $node) {
      $path = _ocs_admin_build_path( $subgroups, $node);
      $depth = count( $path);
      $depthlist[ $depth][] = array(
        'db' => $node,
        'title' => $node['group_name'],
        'mlid' => $node['group_id'],
        'plid' => $node['parent_group_id'],
        'depth' => $depth,
        'path' => $path,
        'below' => array(),
      );
  }
//  dpm( $depthlist, 'depthlist');

  // build tree
  $depth = 1;
  $tree = array();
  while( isset( $depthlist[$depth])) {
    // check each depth as one by one
    foreach( $depthlist[ $depth] as $item) {
        // traverse path
  //      dpm( $item['item']['path'], 'path');
        _ocs_admin_build_tree( $tree, $item['path'], $item);
    }

    $depth = $depth +1;
  }

    dpm( $tree, 'tree');

  $form = array_merge($form, _ocs_group_tree_form($tree));

  if (element_children($form)) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    );
  }
  else {
    $form['#empty_text'] = t('There are no subgroups yet. <a href="@link">Add Subgroup</a>.', 
        array('@link' => url('ocs_admin/group/'. $group['group_id'] .'/add')));
  }

  return $form;
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

function ocs_admin_group_title( $op, $group)
{
    if ( $group['parent_group_id'] == 0)
        return $op. ' Group';
    return $op. ' Sub Group ';
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
function theme_ocs_admin_group_form($variables) {
  $form = $variables['form'];

  drupal_add_tabledrag('menu-overview', 'match', 'parent', 'menu-plid', 'menu-plid', 'menu-mlid', TRUE, MENU_MAX_DEPTH - 1);
  drupal_add_tabledrag('menu-overview', 'order', 'sibling', 'menu-weight');

  $header = array(
    t('Group Name'),
    t('Weight'),
    array( 'data' => t('Operations'), 'colspan' => '3'),
  );

  $rows = array();
  foreach (element_children($form) as $mlid) {
    if (isset( $form[$mlid]['mlid'])) {
    /*
    dpm( $mlid, 'mlid');
    dpm( $form[$mlid], 'form');
    */
      $element = &$form[$mlid];

      $operations = array();
      foreach( element_children( $element['operations']) as $op) {
          $operations[] = array( 'data' => drupal_render($element['operations'][$op]),
            'class' => array('menu-operations'));
      }

      // Add special classes to be used for tabledrag.js.
      $element['plid']['#attributes']['class'] = array('menu-plid');
      $element['mlid']['#attributes']['class'] = array('menu-mlid');
      $element['weight']['#attributes']['class'] = array('menu-weight');

      // Change the parent field to a hidden. This allows any value but hides the field.
      $element['plid']['#type'] = 'hidden';

      $row = array();
      $row[] = theme('indentation', array('size' => $element['#item']['depth'] - 1)) . drupal_render($element['title']);
      $row[] = drupal_render($element['weight']) . drupal_render($element['plid']) . drupal_render($element['mlid']);
      $row = array_merge( $row, $operations);

      $row = array_merge(array('data' => $row), $element['#attributes']);
      $row['class'][] = 'draggable';
      $rows[] = $row;
    }
  }
  $output = '';
  if (empty($rows)) {
    $rows[] = array(array('data' => $form['#empty_text'], 'colspan' => '7'));
  }
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'menu-overview')));
  $output .= drupal_render_children($form);
  return $output;
}


/*
** delete subgroup
*/
function ocs_admin_group_delete_subgroup_confirm( $form, &$form_state, $group)
{
    $form['#group'] = $group;
    $num_members = 1;
    $caption = '';
    if ( $num_members) {
        $caption .= '<p>' . format_plural( $num_members, 
            '<strong>Warning:</strong> There is currently 1 member in "%name". It will be deleted.', 
            '<strong>Warning:</strong> There are currently @count members in "%name". They will be deleted.', 
            array( '%name' => $group['group_name'])) . '</p>';
    }

    $caption .= '<p>'. t('This action cannot be undone.') . '</p>';

    $redirect = 'ocs_admin/group/';
    $redirect .= ($group['parent_group_id'] == 0) ? '' : $group['ancestor_group_id'];

    return confirm_form( $form, t('Are you sure to delete the group "%name"?',
        array( '%name' => $group['group_name'])),
        $redirect, $caption, t('Delete'));
}

function ocs_admin_group_delete_subgroup_confirm_submit( $form, &$form_state)
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
function ocs_admin_group_member_form( $form, &$form_state, $group = NULL, $records = NULL)
{
//    dpm( $records, 'members');

    $form['group_id'] =  array(
        '#type' => 'hidden',
        '#value' => $group['group_id'],
    );

    $form['ancestor_group_id'] =  array(
        '#type' => 'hidden',
        '#value' => $group['ancestor_group_id'],
    );

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

    $header = array( 'account_key' => 'Account Key', 'customer_name' => 'Cutomer Name');
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

function ocs_admin_group_member_form_submit( $form, &$form_state)
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
function ocs_admin_member_add( $group = NULL, $arg = NULL)
{
    if ( $arg ) {
        $_SESSION['ocs_admin_member_add'][ $arg['account_key'] ] = $arg;
    }
    else {
        $_SESSION['ocs_admin_member_add'] = array();
    }

    $form = drupal_get_form( 'ocs_admin_group_member_form', $group, $_SESSION['ocs_admin_member_add']);
    $output = '<div id="ocs_admin_member_add_form_div">' . drupal_render( $form) . '</div>';

    return $output;
}

/*
**  ajax select button in account & product table
    impl: _panel, _popup, and etc
*/
function ocs_member_select( $group, $js = NULL, $type = 'customer', $id = NULL, $name = NULL)
{
//    dpm( $group, 'member select group');

    $arg = array( 
        'account_key' => $id, 
        'customer_name' => $name,
        $id => $id,
    ); 

    $output = ocs_admin_member_add( $group, $arg);

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

    $form['usage'] = array(
        '#type' => 'textfield',
        '#title' => 'Usage',
        '#description' => 'Current Usage of this member',
        '#default_value' => $account['usage'],
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
function ocs_admin_member_delete_confirm( $form, &$form_state, $group, $account_key)
{
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


