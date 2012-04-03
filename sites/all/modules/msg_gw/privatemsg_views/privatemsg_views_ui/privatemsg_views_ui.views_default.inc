<?php

/**
 * @file
 * Default views for the privatemsg_views_ui module.
 */

/**
 * Implementation of hook_views_default_views().
 */
function privatemsg_views_ui_views_default_views() {
  $views = array();
  $path = drupal_get_path('module', 'privatemsg_views_ui');
  include_once($path . '/default_views/privatemsg_mailbox.inc');
  $views[$view->name] = $view;
  include_once($path . '/default_views/privatemsg_thread.inc');
  $views[$view->name] = $view;
  return $views;
}