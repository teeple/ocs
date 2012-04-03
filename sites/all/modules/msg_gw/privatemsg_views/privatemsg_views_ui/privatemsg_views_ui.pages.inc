<?php

/**
 * @file
 * Pages processing of Privatemsg Views UI module.
 */

/**
 * Page callback for the private message thread page (list of messages).
 */
function privatemsg_views_ui_thread($thread) {
  drupal_add_css(drupal_get_path('module', 'privatemsg_views_ui') .'/theme/css/privatemsg-views-ui.css');
  $output = theme('privatemsg_views_ui_thread', $thread);
  if ($thread['read_all']) {
    // User has permission to read all messages AND is not a participant of the current thread.
    drupal_set_message(t('This conversation is being viewed with escalated priviledges and may not be the same as shown to normal users.'), 'warning');
  }
  return $output;
}

/**
 * Page callback for the private message mailbox page (list of threads).
 */
function privatemsg_views_ui_mailbox($uid = NULL) {
  if (!$uid) {
    $uid = $GLOBALS['user']->uid;
  }
  drupal_add_css(drupal_get_path('module', 'privatemsg_views_ui') .'/theme/css/privatemsg-views-ui.css');
  return theme('privatemsg_views_ui_mailbox', $uid);
}

/**
 * Settings form for the Privatemsg Views UI module.
 */
function privatemsg_views_ui_settings_form() {
  $options = array();
  $default_views = views_get_all_views();
  if (is_array($default_views)) {
    foreach ($default_views as $key => $view) {
      // TODO: Add "pm_thread" table here when it's implemented.
      if ($view->base_table == 'pm_message') {
        $options[$key] = $view->name;
      }
    }
  }
  $form['privatemsg_views_ui_mailbox_view'] = array(
    '#type' => 'select',
    '#title' => t('Privatemsg mailbox view'),
    '#options' => $options,
    '#description' => t('A view to use when displaying private message mailbox. The view must accept user ID argument.'),
    '#default_value' => variable_get('privatemsg_views_ui_mailbox_view', 'privatemsg_mailbox'),
  );
  $form['privatemsg_views_ui_thread_view'] = array(
    '#type' => 'select',
    '#title' => t('Privatemsg thread view'),
    '#options' => $options,
    '#description' => t('A view to use when displaying private messages thread. The view must accept thread ID argument.'),
    '#default_value' => variable_get('privatemsg_views_ui_thread_view', 'privatemsg_thread'),
  );
  
  return system_settings_form($form);
}
