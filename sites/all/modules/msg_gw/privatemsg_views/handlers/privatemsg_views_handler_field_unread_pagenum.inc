<?php

/**
 * @file
 * Handler for the "unread page number" pseudo-field.
 * 
 */
class privatemsg_views_handler_field_unread_pagenum extends views_handler_field {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['uid'] = array('table' => 'pm_index', 'field' => 'uid');
    $this->additional_fields['thread_id'] = array('table' => 'pm_index', 'field' => 'thread_id');
  }
    
  /**
   * Load page number data in pre_render stage when main data is already 
   * available.
   */
  function pre_render($values) {
    $thread_alias = $this->aliases['thread_id'];
    $uid_alias = $this->aliases['uid'];
    $rows = array();
    // Usually we have single $uid since we work inside mailbox of a single user
    // but this should handle multiple values too.
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $rows[$value->{$uid_alias}][$value->{$thread_alias}] = $value->{$this->field_alias};
      }
    }
    if (empty($rows)) {
      return;
    }
    // Fetch page numbers.
    foreach ($rows as $uid => $threads ) {
      $this->items[$uid] = privatemsg_views_unread_page_count($uid, $threads);
    }
  }

  function render($values) {
    $thread_id = $values->{$this->aliases['thread_id']};
    $uid = $values->{$this->aliases['uid']};
    $pagenum = $this->items[$uid][$thread_id];
    return check_plain($pagenum);
  }
  
  /**
   * Called to add the field to a query.
   */
  function query() {
    $this->ensure_my_table();
    // Note: Do not add this field to the query.
    $this->field_alias = $this->real_field; 
    $this->add_additional_fields();
  }
  
}
