<?php

/**
 * A handler to display "updated timestamp" aggregate field.
 */
class privatemsg_views_handler_field_updated extends views_handler_field_date {
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
        'max' => t('Last message (max message timestamp)'),
        'mix' => t('First message (min message timestamp)'),
      ),
      '#default_value' => $this->options['type'],
    );
  }
  
  function query() {
    $this->ensure_my_table();
    // Add the field.
    switch ($this->options['type']) {
      case 'max':
        $formula = "MAX(pm_message.timestamp)";
        break;
      case 'min':
        $formula = "MIN(pm_message.timestamp)";
        break;
    }
    $this->field_alias = $this->query->add_field(NULL, $formula, 'updated', array('aggregate' => TRUE));
    $this->add_additional_fields();
    
  }  
}
