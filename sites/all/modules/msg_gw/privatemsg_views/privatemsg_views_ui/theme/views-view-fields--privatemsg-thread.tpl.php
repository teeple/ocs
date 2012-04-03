<?php

/**
 * @file
 * Privatemsg thread view fields style row template.
 *
 * - $view: The view in use.
 * - $fields: an array of $field objects. Each one contains:
 *   - $field->content: The output of the field.
 *   - $field->raw: The raw data for the field, if it exists. This is NOT output safe.
 *   - $field->class: The safe class id to use.
 *   - $field->handler: The Views field handler object controlling this field. Do not use
 *     var_export to dump this object, as it can't handle the recursion.
 *   - $field->inline: Whether or not the field should be inline.
 *   - $field->inline_html: either div or span based on the above flag.
 *   - $field->separator: an optional separator that may appear before a field.
 * - $row: The raw result object from the query, with all data it fetched.
 * - $new_anchor: unique id of the named anchor element for new messages: 
 *   "new", "new-2", "new-3", etc.
 *   
 * @ingroup views_templates
 *
 */
?>

<div class="privatemsg-views-message">
<div class="privatemsg-thread-left">
  <div class="views-field-<?php print $fields['picture']->class; ?>">
    <?php print $fields['picture']->content; ?>
  </div>
  <div class="views-field-<?php print $fields['name']->class; ?>">
    <?php print $fields['name']->content; ?>
  </div>
</div>

<div class="privatemsg-thread-right">
  <span class="views-field-<?php print $fields['timestamp']->class; ?>">
    <?php print $fields['timestamp']->content; ?>
  </span>
  
  <?php if ($fields['is_new']->raw == 1): ?>
    <span class="views-field-<?php print $fields['is_new']->class; ?>">
      <a class="marker" id="<?php print $new_anchor ?>"><?php print t('new'); ?></a>
    </span>
  <?php endif;?>
    
  <div class="views-field-<?php print $fields['body']->class; ?>">
    <?php print $fields['body']->content; ?>
  </div>
  
  <div class="views-field-<?php print $fields['message_actions']->class; ?>">
    <?php print $fields['message_actions']->content; ?>
  </div>
  
  
</div>
<div class="privatemsg-views-message-bottom"></div>
</div>