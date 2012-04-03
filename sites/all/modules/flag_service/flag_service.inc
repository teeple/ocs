<?php

/**
 * @file
 * Services integration for the Flag module.
 */

/**
 * Access callback to check a user has access to a flag operation via Services.
 *
 * @param $flag_name
 *   The flag name.
 * @param $content_id
 *   The content ID that should be flagged.
 * @param $uid
 *   Optional. The user ID on behalf to flag the content.
 * @param $action
 *   Optional. The desired method - flag or unflag. Defaults to flag.
 * @return
 *   TRUE if access is allowed.
 */
function flag_service_flag_content_access($flag_name, $content_id, $action = 'flag', $uid = NULL) {
  // Check action is valid.
  if (!in_array($action, array('flag', 'unflag'))) {
    return FALSE;
  }

  $account = NULL;
  if (!empty($uid)) {
    $account = user_load($uid);
  }
  
  $flag = flag_get_flag($flag_name);

  if (!$flag) {
    // Flag does not exist.
    return services_error(t('There is no file with flag with the name @flag_name', array('@flag_name' => $flag_name)), 406);
  }

  if (!$flag->access($content_id, $action, $account)) {
    // User has no permission to use this flag.
    return FALSE;
  }

  return TRUE;
}

/**
 * Service wrapper to flag a content.
 *
 * @param $flag_name
 *   The flag name.
 * @param $content_id
 *   The content ID to check if it was flagged.
 * @param $uid
 *   The user ID to check if they flagged the content.
 * @param $action
 *   The action. Should be "flag" or "unflag".
 * @return
 *   TRUE if content was flagged.
 */
function flag_service_flag_content($flag_name, $content_id, $action = 'flag', $uid = NULL, $skip_permission_check = FALSE) {
  global $user;
  $account = empty($uid) ? $user : user_load($uid);
  $flag = flag_get_flag($flag_name);
  $skip_permission_check = (boolean) $skip_permission_check;
  if (!$flag) {
    watchdog('services', '<pre>' . print_r(debug_backtrace(), TRUE) . '</pre>');
    // Flag does not exist.
    return services_error(t('There is no file with flag with the name @flag_name', array('@flag_name' => $flag_name)), 406);
  }
  return $flag->flag($action, $content_id, $account, $skip_permission_check);
}

/**
 * Service wrapper to check if a content is flagged by a user.
 *
 * @param $flag_name
 *   The flag name.
 * @param $content_id
 *   The content ID to check if it was flagged.
 * @param $uid
 *   The user ID to check if they flagged the content.
 * @return
 *   TRUE if content was flagged.
 */
function flag_service_is_flagged($flag_name, $content_id, $uid = NULL) {
  $flag = flag_get_flag($flag_name);

  if (!$flag) {
    // Flag does not exist.
    return services_error(t('There is no file with flag with the name @flag_name', array('@flag_name' => $flag_name)), 406);
  }
  return $flag->is_flagged($content_id, $uid);
}

/**
 * Service wrapper to count a flag on a content.
 *
 * @param $flag_name
 *   The flag name.
 * @param $content_id
 *   The content ID to check if it was flagged.
 * @return
 *   A object contain the number which shows how many users flagged a content.
 */

function flag_service_flag_countall($flag_name, $content_id) {
  $query = db_select('flag_counts', 'fc');
  $query->join('flags', 'f','f.fid = fc.fid');
  $query->fields('fc', array('count'))
        ->condition('fc.content_id', $content_id)
        ->condition('f.name', $flag_name);
  $result = $query->execute()->fetchObject();
  return $result;                  
}

