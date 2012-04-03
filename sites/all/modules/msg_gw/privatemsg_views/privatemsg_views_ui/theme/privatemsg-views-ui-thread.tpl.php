<?php

/**
 * @file
 * Privatemsg thread (a list of messages) template.
 * 
 * Available variables:
 * - $thread: array with thread properties. WARNING: properties are unsafe for 
 *   raw output!
 * - $subject: themed thread subject
 * - $participants: themed participant list
 * - $thread_view: themed thread view
 * - $reply_form: themed reply form
 */
?>

<div>
<span><?php print t('Subject') .':'; ?></span>
<span><?php print $subject; ?></span>
</div>
<div>
<span><?php print t('Participants') .':'; ?></span>
<span><?php print $participants; ?></span>
</div>
<?php print $thread_view; ?>
<?php print $reply_form; ?>