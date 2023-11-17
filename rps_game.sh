#!/bin/bash

echo "じゃんけんゲームを始めます！"
echo "1: グー, 2: チョキ, 3: パー を選んでください。"
echo -n "選択してください（1-3）: "
read player_choice

# プレーヤーの選択を表示
case $player_choice in
    1) player_hand="グー";;
    2) player_hand="チョキ";;
    3) player_hand="パー";;
    *) echo "無効な選択です。"; exit 1;;
esac
echo "あなたの選択: $player_hand"

# コンピューターの手をランダムで選択
computer_choice=$(( RANDOM % 3 + 1 ))

# コンピューターの選択を表示
case $computer_choice in
    1) computer_hand="グー";;
    2) computer_hand="チョキ";;
    3) computer_hand="パー";;
esac
echo "コンピューターの選択: $computer_hand"

# じゃんけんの勝敗を判定
if [ $player_choice -eq $computer_choice ]; then
    echo "引き分けです！"
elif [ $player_choice -eq 1 -a $computer_choice -eq 2 ] || [ $player_choice -eq 2 -a $computer_choice -eq 3 ] || [ $player_choice -eq 3 -a $computer_choice -eq 1 ]; then
    echo "あなたの勝ちです！"
else
    echo "コンピューターの勝ちです！
fi