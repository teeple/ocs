<?php

/**
 * A handler to display "is blocked" field.
 */
class privatemsg_views_handler_field_is_blocked extends views_handler_field {
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
        'numeric' => t('1/0'),
        'yes-no' => t('"Yes"/"No"'),
        'blocked-empty' => t('"blocked"/empty'),
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
    $value = !empty($values->{$this->field_alias});
    if (!empty($this->options['not'])) {
      $value = !$value;
    }
    switch ($this->options['type']) {
      default:
      case 'yes-no':
        return $value ? t('Yes') : t('No');
      case 'blocked-empty':
        return $value ? t('blocked') : '';
      case 'numeric':
        return check_plain($value);
    }
  }
}
