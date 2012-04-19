<?php

/**
 * Set count variables so column numbers can be dynamic.
 */
function urbansolice_preprocess_page (&$vars) {
  $vars['preface'] = count(array_filter(array($vars['page']['preface_one'], $vars['page']['preface_two'], $vars['page']['preface_three'])));
  $vars['bottom'] = count(array_filter(array($vars['page']['bottom_one'], $vars['page']['bottom_two'], $vars['page']['bottom_three'], $vars['page']['bottom_four'])));
  // Display user account links.
  $vars['user_links'] = _urbansolice_user_links();
  //dpm($vars);
}

/**
 * User/account related links.
 */
function _urbansolice_user_links() {
  // Add user-specific links
  global $user;
  $user_links = array();
  if (empty($user->uid)) {
    $user_links['login'] = array('title' => t('Login'), 'href' => 'user');
    // Do not display register link if registration is not allowed.
    if (variable_get('user_register', 1)) {
      $user_links['register'] = array('title' => t('Register'), 'href' => 'user/register');
    }
  }
  else {
    $user_links['account'] = array('title' => t('Hello @username', array('@username' => $user->name)), 'href' => 'user', 'html' => TRUE);
    $user_links['logout'] = array('title' => t('Logout'), 'href' => "logout");
  }
  return $user_links;
}
