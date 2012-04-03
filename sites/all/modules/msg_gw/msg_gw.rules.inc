<?php

function tvchat_rules_event_info() {
    return array(
        'account_reset' => array(
            'label' => t('account reset (비밀번호 재생성)'),
            'module' => 'tvchat',
            'arguments' => array(
                'account' => array('type' => 'user', 'label' => t('Account, account_reset된 사용자')),
                'password_tmp' => array('type' => 'string', 'label' => t('Passowrd, account_reset된 사용자의 이메일 비밀번호'))
            ),
        ),
        'push_bulk_message' => array(
            'label' => t('send bulk push message (메시지 다중 전송)'),
            'module' => 'tvchat',
            'arguments' => array(
                'message' => array('type' => 'string', 'label' => t('message to be pushed')),
                'recipient_uids' => array('type' => 'string', 'label' => t('Account, 받는 이들의 id 집합'))
            ),
         ),
    );
}

