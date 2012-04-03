<?php

/**
 * @file
 * Theme preprocessing of Privatemsg Views UI module.
 */

/**
 * Preprocess mailbox view page callback variables.
 */
function template_preprocess_privatemsg_views_ui_mailbox(&$variables) {
  $view_name = variable_get('privatemsg_views_ui_mailbox_view', 'privatemsg_mailbox');
  $variables['mailbox_view'] = _privatemsg_views_ui_embed_view($view_name, 'default', FALSE, $variables['uid']);
}

/**
 * Preprocess thread view page callback variables.
 */
function template_preprocess_privatemsg_views_ui_thread(&$variables) {
  $thread = $variables['thread'];
  $thread_id = $thread['thread_id'];
  
  // Add subject.
  $variables['subject'] = check_plain($thread['subject']);
  
  // Add participants.
  // Use our $participants and not $thread['participants'] because we order
  // participants differently.
  $participants = privatemsg_views_get_participants($thread_id);
  $variables['participants'] = theme('privatemsg_views_participants', $participants[$thread_id]);
  
  // Add view
  $view_name = variable_get('privatemsg_views_ui_thread_view', 'privatemsg_thread');
  $variables['thread_view'] = _privatemsg_views_ui_embed_view($view_name, 'default', 'messages/view/%', $thread_id); 
  
  // Add reply form.
  if (privatemsg_user_access('write privatemsg')) {
    module_load_include('inc', 'privatemsg', 'privatemsg.pages');
    $variables['reply_form'] = drupal_get_form('privatemsg_new', $thread['participants'], $thread['subject'], $thread['thread_id'], $thread['read_all']);
  }
}

/**
 * Preprocess private message field variables (row template).
 */
function template_preprocess_views_view_fields__privatemsg_thread(&$variables) {
  static $num;
  $fields = $variables['fields'];
  if (isset($fields['is_new']) && $fields['is_new']->raw == 1) {
    if (!isset($num)) {
      $num = 1;
      $variables['new_anchor'] = "new";
    }
    else {
      $num++;
      $variables['new_anchor'] = "new-$num";
    }
    // Set message as "read" only if a user reads his own mailbox messages.
    if (isset($fields['uid']) && isset($fields['mid']) && $GLOBALS['user']->uid == $fields['uid']->raw) {
      privatemsg_message_change_status($fields['mid']->raw, PRIVATEMSG_READ);
    }
  }
}



