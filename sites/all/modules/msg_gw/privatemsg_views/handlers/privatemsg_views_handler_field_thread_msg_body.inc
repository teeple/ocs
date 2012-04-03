<?php

/**
 * @file
 * Handler for the "thread message body" field for mailbox views.
 * 
 */
class privatemsg_views_handler_field_thread_msg_body extends views_handler_field {
  /**
   * Load message body data in pre_render stage when main data is already 
   * available.
   */
  function pre_render($values) {
    // Prepare mid list for IN list.
    $mids = array();
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $mids[] = $value->{$this->field_alias};
      }
    }
    if (empty($mids)) {
      return;
    }
    $query = "SELECT * FROM {pm_message} m WHERE m.mid IN(" . db_placeholders($mids) .")";
    $result = db_query($query, $mids);
    while ($message = db_fetch_object($result)) {
      $this->items[$message->mid] = $message;
    }
  }
  
  /**
   * Override render method to use bodies fetched in the pre-render stage.  
   */
  function render($values) {
    $value = $values->{$this->field_alias};
    if ($value) {
      $body = $this->items[$value]->body;
      $format = $this->items[$value]->format;
      return check_markup($body, $format, FALSE);
    }
  }
  
  /**
   * Do not add real field to the query. We need this because our real field is
   * an aliased formula value and Views can't guess it itself. 
   */
  function query() {
    $this->ensure_my_table();
    // Note: Do not add this field to the query.
    $this->field_alias = $this->real_field;
    $this->add_additional_fields();
  }
}
