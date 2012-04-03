<?php

/**
 * @file
 * Handler for the "participants" field of private message thread.
 * 
 */
class privatemsg_views_handler_field_participants extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_user'] = array('default' => TRUE);
    $options['replace_current'] = array('default' => FALSE);    
    return $options;
  }

  /**
   * Provide "link to profile" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_user'] = array(
      '#title' => t('Link usernames to profiles'),
      '#description' => t('When enabled, it will show usernames as links to user profiles'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['link_to_user'],
    );
    $form['replace_current'] = array(
      '#type' => 'checkbox',
      '#title' => t('Output current user differently'),
      '#description' => t('If enabled, current user name in the list will be replaced with the output of theme_privatemsg_views_self() function ("me" by default).'),
      '#default_value' => $this->options['replace_current'],
    );
  }

  /**
   * Load participant data in pre_render stage when main data is already 
   * available.
   */
  function pre_render($values) {
    $threads = array();
    foreach ($values as $value) {
      if (!empty($value->{$this->field_alias})) {
        $threads[$value->{$this->field_alias}] = $value->{$this->field_alias};
      }
    }
    // Fetch participants.
    if (!empty($threads)) {
      $this->items = privatemsg_views_get_participants($threads);  
    }
  }

  function render($values) {
    $participants = $this->items[$values->{$this->field_alias}];
    return theme('privatemsg_views_participants', $participants, $this->options['link_to_user'], $this->options['replace_current']);
  }
}
