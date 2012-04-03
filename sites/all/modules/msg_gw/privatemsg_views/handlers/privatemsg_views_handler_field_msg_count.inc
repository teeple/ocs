<?php

/**
 * @file
 * Handler for the "message count" field of private message thread.
 * 
 */
class privatemsg_views_handler_field_msg_count extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['uid'] = array('table' => 'pm_index', 'field' => 'uid');
  }
  
  function option_definition() {
    $options = parent::option_definition();
    $options['count_filter'] = array('default' => 'not_deleted');    
    return $options;
  }
  
  /**
   * Provide "link to profile" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $options = array(
      0 => t("don't care"),
      1 => t('yes'),
      2 => t('no'),
    );
    $form['count_messages_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Count messages'),
    );
    $form['count_deleted'] = array(
      '#title' => t('Deleted'),
      '#prefix' => t('Please select which messages to count.'),
      '#description' => t('Count deleted messages ?'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $this->options['count_deleted'],
    );
    $form['count_new'] = array(
      '#title' => t('New'),
      '#description' => t('Count new (unread) messages ?'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $this->options['count_new'],
    );
    if (module_exists('pm_block_user')) {
      $form['count_blocked'] = array(
        '#title' => t('Blocked'),
        '#description' => t('Count blocked messages ?'),
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $this->options['count_blocked'],
      );
    }
    
    
  }
  
  /**
   * Load message count data in pre_render stage when main data is already 
   * available.
   */
  function pre_render($values) {
    // Prepare thread list for IN list.
    $threads = array();
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $threads[] = $value->{$this->field_alias};
      }
    }
    if (empty($threads)) {
      return;
    }
    
    $query = array();
    $query['select'] = array();
    $query['where'] = array();
    $query['groupby'] = array();
    
    $query['select'][] = "SELECT pmi.thread_id, pmi.uid, COUNT(pmi.mid) AS msg_count FROM {pm_index} pmi";
     
    if (module_exists('pm_block_user') && $this->options['count_blocked'] > 0) {
      $query['select'][] = "LEFT JOIN {pm_message} m ON pmi.mid = m.mid";
      $query['select'][] = "LEFT JOIN {pm_block_user} pmbu ON pmi.uid = pmbu.recipient AND m.author = pmbu.author";
      if ($this->options['count_blocked'] == 1) {
        $query['where'][] = "pmbu.recipient IS NOT NULL";
      }
      if ($this->options['count_blocked'] == 2) {
        $query['where'][] = "pmbu.recipient IS NULL";
      }
    }
    
    $query['where'][] = "pmi.thread_id IN (" . db_placeholders($threads) . ")";
    if ($this->options['count_deleted'] == 1) {
      $query['where'][] = "pmi.deleted > 0";
    }
    if ($this->options['count_deleted'] == 2) {
      $query['where'][] = "pmi.deleted = 0";
    }
    if ($this->options['count_new'] == 1) {
      $query['where'][] = "pmi.is_new = 1";
    }
    if ($this->options['count_new'] == 2) {
      $query['where'][] = "pmi.is_new = 0";
    }
    $query['groupby'][] = "pmi.thread_id";
    $query['groupby'][] = "pmi.uid";
    
    $query_string  = implode(' ', $query['select']);
    $query_string .= ' WHERE (' . implode(') AND (', $query['where']) . ')';
    $query_string .= ' GROUP BY ' . implode(',', $query['groupby']);
    
    $result = db_query($query_string, $threads);
    while ($thread = db_fetch_object($result)) {
      // Add uid key so every participant gets own msg count.
      $this->items[$thread->thread_id][$thread->uid][0]['msg_count'] = $thread->msg_count;
      $this->items[$thread->thread_id][$thread->uid][0]['msg_count_raw'] = $thread->msg_count;
    }
  }
  
  function get_items($values) {
    $uid = $values->{$this->aliases['uid']};
    $thread_id = $values->{$this->field_alias};
    if (!empty($this->items[$thread_id][$uid])) {
      return $this->items[$thread_id][$uid];
    }
    return array(0 => array('msg_count' => 0));
  }
  
  function render_item($count, $item) {
    return $item['msg_count'];
  }
  
  function document_self_tokens(&$tokens) {
    $id = $this->options['id'];
    $tokens['[' . $id . '-raw' . ']'] = t('raw number of messages.');
    $tokens['[' . $id . '-yes-no' . ']'] = t('"Yes" or "No" depending on if number is > 0 or 0.');
    $tokens['[' . $id . '-string' . ']'] = t('number as a string e.g. "5 messages".');
    $tokens['[' . $id . '-new-empty' . ']'] = t('"#new" or empty string depending on if number is > 0 or 0. Useful for anchor fragments.');
    $tokens['[' . $id . '-new-string' . ']'] = t('number of messages as new messages count e.g. "5 new messages"');
    $tokens['[' . $id . '-deleted-string' . ']'] = t('number of messages as deleted messages count e.g. "5 deleted messages"');
  }

  function add_self_tokens(&$tokens, $item) {
    $count = $item['msg_count_raw'];
    $id = $this->options['id'];   
    $tokens['[' . $id . '-raw' . ']'] = $count > 0 ? $count : "0";
    $tokens['[' . $id . '-yes-no' . ']'] = $count > 0 ? t('Yes') : t('No');
    $tokens['[' . $id . '-string' . ']'] = $count > 0 ? format_plural($count, '1 message', '@count messages') : '';
    $tokens['[' . $id . '-new-empty' . ']'] = $count > 0 ? '#new' : '';
    $tokens['[' . $id . '-new-string' . ']'] = $count > 0 ? format_plural($count, '1 new message', '@count new messages') : '';
    $tokens['[' . $id . '-deleted-string' . ']'] = $count > 0 ? format_plural($count, '1 deleted message', '@count deleted messages')  : '';
  }
  
}
