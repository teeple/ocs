<?php

/**
 * A handler to display "is new" field.
 */
class privatemsg_views_handler_field_is_new extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['type'] = array('default' => 'yes-no');
    $options['not'] = array('definition bool' => 'reverse');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['type'] = array(
      '#type' => 'select',
      '#title' => t('Output format'),
      '#options' => array(
        'raw' => t('1/0'),
        'yes-no' => t('"Yes"/"No"'),
        'new-empty' => t('"new"/empty'),
      ),
      '#default_value' => $this->options['type'],
    );
    $form['not'] = array(
      '#type' => 'checkbox',
      '#title' => t('Reverse'),
      '#description' => t('If checked, true will be displayed as false.'),
      '#default_value' => $this->options['not'],
    );
  }

  function render($values) {
    $value = $values->{$this->field_alias};
    if (!empty($this->options['not'])) {
      $value = !$value;
    }
    switch ($this->options['type']) {
      default:
      case 'yes-no':
        return $value ? t('Yes') : t('No');
      case 'new-empty':
        return $value ? t('new') : '';
      case 'raw':
        return check_plain($value);
    }
  }
}
