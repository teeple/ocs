<?php

/**
 * A handler to display "message actions" field.
 * 
 * TODO: We may need to insert participant checks too, to control/block
 * message actions when a privileged user views a thread on behalf of another
 * user.  
 */
class privatemsg_views_handler_field_message_actions extends views_handler_field {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['author'] = array('table' => 'pm_message', 'field' => 'author');
    $this->additional_fields['thread_id'] = array('table' => 'pm_index', 'field' => 'thread_id');
    if (module_exists('pm_block_user')) {
      $this->additional_fields['is_blocked'] = array('table' => 'pm_block_user', 'field' => 'recipient');
    }
  }
  
  function render($values) {
    global $user;
    $mid = $values->{$this->field_alias};
    $thread_id = $values->{$this->aliases['thread_id']};
    $author = $values->{$this->aliases['author']};
    if (module_exists('pm_block_user')) {
      $is_blocked = $values->{$this->aliases['is_blocked']};
    }
    
    $links = array();
    
    // Delete message action.
    if (privatemsg_user_access('delete privatemsg')) {
      $links['delete_message'] = array('title' => t('Delete message'), 'href' => 'messages/delete/' . $thread_id . '/' . $mid);
    }
    
    // PM Block User actions.
    if (module_exists('pm_block_user')) {
      if ($user->uid != $author) {
        if (!empty($is_blocked)) {
          $links['unblock_author'] = array(
            'title' => t('Unblock author'),
            'href' => 'messages/block/'. $author,
            'query' => 'destination=messages/view/' . $thread_id
          );
        }
        else {
          $links['block_author'] = array(
            'title' => t('Block author'),
            'href' => 'messages/block/'. $author,
            'query' => 'destination=messages/view/' . $thread_id
          );
        }
      }
    }
    
    drupal_alter('privatemsg_views_message_action_links', $links, $values, $this);
    if (!empty($links)) {
      return theme('privatemsg_views_message_action_links', $links, array('class' => 'message-actions'));
    }
    else {
      return '';  
    }
  }
}
