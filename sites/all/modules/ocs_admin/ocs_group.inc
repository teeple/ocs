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
        $row[] = array( 'data' => l(t('list members'), 'ocs_admin/group/manage'. $item['group_id']));
        $row[] = array( 'data' => l(t('list members'), 'ocs_admin/group/manage'. $item['group_id']));
        $row[] = array( 'data' => l(t('list members'), 'ocs_admin/group/manage'. $item['group_id']));
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
        '#value' => $group['group_id'],
    );

    $form['parent_group_id'] = array(
        '#type' => 'hidden',
        '#value' => $group['parent_group_id'],
    );

    $form['ancestor_group_id'] = array(
        '#type' => 'hidden',
        '#value' => $group['ancestor_group_id'],
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

function ocs_admin_group_edit_form_submit( $form, &$form_state)
{
    $group = $form_state['values'];
    dpm( $group, 'submit group');
    ocs_admin_group_save($group);

    drupal_set_message( t('Your configuration has been saved.'));

    $form_state['redirect'] = 'ocs_admin/group/manage/' . $group['ancestor_group_id'];
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

    $form_state['redirect'] = 'ocs_admin/group/manage/' . $group['ancestor_group_id'];
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
      $node['path'] = _ocs_admin_build_path( $subgroups, $node);
      $node['depth'] = count( $node['path']);
      $depthlist[ $node['depth']][] = array(
        'item' => $node,
        'title' => $node['group_name'],
        'mlid' => $node['group_id'],
        'plid' => $node['parent_group_id'],
        'depth' => $node['depth'],
        'below' => array(),
      );
  }
  dpm( $depthlist, 'depthlist');

  // build tree
  $depth = 1;
  $tree = array();
  while( isset( $depthlist[$depth])) {
    // check each depth as one by one
    foreach( $depthlist[ $depth] as $item) {
        // traverse path
  //      dpm( $item['item']['path'], 'path');
        _ocs_admin_build_tree( $tree, $item['item']['path'], $item);
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
        array('@link' => url('ocs_admin/group/manage/'. $group['groupid'] .'/add')));
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
        '#href' => 'ocs_admin/group/manage/'. $item['mlid'] . '/edit');
      $operations['delete'] = array( 
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => 'ocs_admin/group/manage/'. $item['mlid'] . '/delete');
      $form[$mlid]['operations'] = $operations;  
    }

    if ( isset($data['below'])) {
      _ocs_group_tree_form($data['below']);
    }
  }
  return $form;
}

function ocs_admin_group_title( $op)
{
    return $op. ' Group';
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
    array( 'data' => t('Operations'), 'colspan' => '2'),
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


