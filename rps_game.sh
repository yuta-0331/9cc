#!/bin/bash

echo "����񂯂�Q�[�����n�߂܂��I"
echo "1: �O�[, 2: �`���L, 3: �p�[ ��I��ł��������B"
echo -n "�I�����Ă��������i1-3�j: "
read player_choice

# �v���[���[�̑I����\��
case $player_choice in
    1) player_hand="�O�[";;
    2) player_hand="�`���L";;
    3) player_hand="�p�[";;
    *) echo "�����ȑI���ł��B"; exit 1;;
esac
echo "���Ȃ��̑I��: $player_hand"

# �R���s���[�^�[�̎�������_���őI��
computer_choice=$(( RANDOM % 3 + 1 ))

# �R���s���[�^�[�̑I����\��
case $computer_choice in
    1) computer_hand="�O�[";;
    2) computer_hand="�`���L";;
    3) computer_hand="�p�[";;
esac
echo "�R���s���[�^�[�̑I��: $computer_hand"

# ����񂯂�̏��s�𔻒�
if [ $player_choice -eq $computer_choice ]; then
    echo "���������ł��I"
elif [ $player_choice -eq 1 -a $computer_choice -eq 2 ] || [ $player_choice -eq 2 -a $computer_choice -eq 3 ] || [ $player_choice -eq 3 -a $computer_choice -eq 1 ]; then
    echo "���Ȃ��̏����ł��I"
else
    echo "�R���s���[�^�[�̏����ł��I
fi