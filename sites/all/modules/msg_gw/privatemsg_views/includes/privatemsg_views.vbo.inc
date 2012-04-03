<?php

/**
 * @file
 * Views Bulk Operations integration for the Privatemsg Views module.
 */

/**
 * Implementation of hook_views_bulk_operations_object_info()
 *
 * Hook used by VBO to be able to handle different objects as does Views 2.
 *
 * The array returned for each object type contains:
 *  'type' => the object type name, should be the same as 'type' field in actions. 
 *  'base_table' => the Views 2 table name corresponding to that object type, should be the same as the $view->base_table attribute.
 *  'load' => a function($id) that returns the corresponding object.
 *  'title' => an attribute on the object that returns a human-friendly identifier of the object.
 *  'access' (optional) => a function($op, $node, $account = NULL) that behaves like node_access().
 *
 */
function privatemsg_views_views_bulk_operations_object_info() {
  return array(
    'pm_thread' => array(
      'type' => 'pm_thread',
      'base_table' => 'pm_message',
      'load' => '_privatemsg_views_thread_load',
      'title' => 'subject',
      //'access' => '_privatemsg_views_thread_access'
    ),
  );
}

function _privatemsg_views_thread_load($mid) {
  $query = 'SELECT DISTINCT(pmi.thread_id) AS id, m.subject FROM {pm_message} m INNER JOIN {pm_index} pmi ON m.mid = pmi.mid WHERE m.mid = %d';
  $result = db_query($query, $mid);
  while ($thread = db_fetch_object($result)) {
    return $thread; 
  }
  return FALSE;
}

/**
 * Implementation of hook_OBJECT_operations() of VBO.
 * 
 * Atm it declares each operation manually. We are not using
 * hook_privatemsg_thread_operation() to transform $operations by inserting
 * our wrapper. Problem is there can be some checks such as 
 * privatemsg_user_access('delete privatemsg') inside that hook, so we could
 * get *incomplete* list of operations while we need complete one. Privatemsg 
 * performs additional checks to decide whether or not to declare operation 
 * because that hook_privatemsg_thread_operation() is invoked during UI 
 * processing for specific user. OTOH, VBO invokes hook_OBJECT_operations() 
 * during Views configuration and user configuring Views+VBO is different 
 * than a user that may invoke the actions.
 * 
 * In short: we are stuck declaring our operations manually.
 */
function privatemsg_views_pm_thread_operations() {
  $operations = array(
    'mark as read' => array(
      'label' => t('Mark as read'),
      'callback' => '_privatemsg_views_thread_operation',
      'callback arguments' => array('callback' => 'privatemsg_thread_change_status', 'status' => PRIVATEMSG_READ),
    ),
    'mark as unread' => array(
      'label' => t('Mark as unread'),
      'callback' => '_privatemsg_views_thread_operation',
      'callback arguments' => array('callback' => 'privatemsg_thread_change_status', 'status' => PRIVATEMSG_UNREAD),
    ),
    'delete' => array(
      'label' => t('Delete'),
      'callback' => '_privatemsg_views_thread_operation',
      'callback arguments' => array('callback' => 'privatemsg_thread_change_delete', 'status' => 1),
    ),
    'undelete' => array(
      'label' => t('Undelete'),
      'callback' => '_privatemsg_views_thread_operation',
      'callback arguments' => array('callback' => 'privatemsg_thread_change_delete', 'status' => 0),
    ),
  );
  return $operations;
}

/**
 * Wrapper that invokes PM thread operation but accepts array of message IDs 
 * instead of thread IDs.
 * 
 * We need this until Privatemsg gets separate table for threads with thread_id
 * primary key.
 */
function _privatemsg_views_thread_operation($mids, $callback, $status) {
  $tids = array();
  $query  = 'SELECT DISTINCT(pmi.thread_id) FROM {pm_index} pmi';
  $query .=  ' WHERE pmi.mid IN (' . db_placeholders($mids) . ')'; 
  $result = db_query($query, $mids);
  while ($thread = db_fetch_object($result)) {
    $tids[] = $thread->thread_id;
  }
  $args = array($tids, $status);
  return call_user_func_array($callback, $args);
}
