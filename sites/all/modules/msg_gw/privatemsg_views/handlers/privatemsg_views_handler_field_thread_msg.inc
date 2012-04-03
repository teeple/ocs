<?php

/**
 * A handler to display "last message id" field.
 */
class privatemsg_views_handler_field_thread_msg extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['type'] = array('default' => 'max');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['type'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => array(
        'max' => t('Last message (max message id)'),
        'mix' => t('First message (min message id)'),
      ),
      '#default_value' => $this->options['type'],
    );
  }
  
  function query() {
    $this->ensure_my_table();

    // Remove "mid" base field to prevent it breaking grouping and because
    // we are going to use same name as an alias.
    unset($this->query->fields['mid']);    
    
    // Add the field.
    switch ($this->options['type']) {
      case 'max':
        $formula = "MAX(pm_message.mid)";
        break;
      case 'min':
        $formula = "MIN(pm_message.mid)";
        break;
    }
    // Use "mid" alias to trick VBO into using it like it was "base field".
    $this->field_alias = $this->query->add_field(NULL, $formula, 'mid', array('aggregate' => TRUE));
            
    $this->add_additional_fields();
  }
}
