#!/usr/bin/python3
from tkinter import *

## マス目の定義
grid_state = [[False for i in range(40)] for j in range(40)]
pen = False
enable = True

def save():
    file = open('arrange.txt', 'w')
    for y in range(len(grid_state)):
        for x in range(len(grid_state[0])):
            if grid_state[y][x]:
                s = str(x) + " " + str(y) + "\n"
                file.write(s)
    file.close()

####メイン画面の生成####
root = Tk()
root.title("マインスイーパ")
root.resizable(0, 0)  # サイズ変更不可にする

###メニュー作成###
# メニューオブジェクトを作る
menu_ROOT = Menu(root)
# メインウィンドウ（root）のmenuに作成したメニューオブジェクトを設定し更新
root.configure(menu=menu_ROOT)

# ゲームメニューを作る。menu_ROOTが親オブジェクト
menu_GAME = Menu(menu_ROOT, tearoff=False)

# 「ゲーム」というラベルで親メニューをrootメニューに追加。
# サブメニュー付きなのでadd_cascadeメソッドを使う
menu_ROOT.add_cascade(label='メニュー', under=4, menu=menu_GAME)
# 「初級」「中級」「上級」ラベルでサブメニューを親メニュー（menu_GAME）に追加
menu_GAME.add_command(label="保存", under=3, command=save)
# 「終了」ラベルでメニューをrootメニューに追加
menu_ROOT.add_command(label="終了", under=3, command=exit)

###フレームオブジェクト作成###
#外枠のフレーム作成
root_frame = Frame(root, relief = 'groove', borderwidth = 5, bg = 'LightGray')
#上部ステータス画面のフレーム作成
status_frame = Frame(root_frame, height = 50, relief = 'sunken', borderwidth = 3, bg = 'LightGray')
#下部ゲーム画面のフレーム作成
game_frame = Frame(root_frame, relief = 'sunken', borderwidth = 3, bg = 'LightGray')
#それぞれのフレームを配置する。ステータス画面とゲーム画面は上下左右それぞれ余白を少し設ける
root_frame.pack()
status_frame.pack(pady = 5, padx = 5, fill = 'x')
game_frame.pack(pady = 5, padx = 5)



####マス目の作成####
# 左クリックした際のイベント関数を定義。これは下で（21行目で）フレームにクリックイベントを定義しているけども、
# それよりも先に書かないとダメ

def right_click(event):
    global enable
    enable = not enable

def left_click(event):
    global pen
    pen = not pen

def enter(event):
    global grid_state
    # event.widgetで該当のオブジェクト（ウィジェット=部品）を取得できる
    # クリックした感を出すので、relefをridgeに変更する
    # grid_state[event.widget.y][event.widget.x] = not grid_state[event.widget.y][event.widget.x]
    if enable:
        grid_state[event.widget.y][event.widget.x] = pen
    if grid_state[event.widget.y][event.widget.x]:
        event.widget.configure(relief='raised', bd='3')
    else:
        event.widget.configure(relief='ridge', bd='1')

    # またそのフレームのアトリビュートnumを表示する
    print(event.widget.x, event.widget.y, ", pen onoff: ", grid_state[event.widget.y][event.widget.x], ", pen enable: ", enable)


# 各マス目に番号を振っておくといろいろ便利なのでそれ用の変数iを定義する
i = 0
# 繰り返し作成したフレーム格納用リスト
frame_list = []
# for文の入れ子構造にして、9×9回繰り返す
for y in range(len(grid_state)):
    for x in range(len(grid_state[0])):
        # タテヨコ30pxの小さいフレームを量産。reliefをraisedにして出っ張り感を再現する
        if grid_state[y][x]:
            frame = Frame(game_frame, width=20, height=20, bd=3, relief='raised', bg='LightGray')
        else:
            frame = Frame(game_frame, width=20, height=20, bd=1, relief='ridge', bg='LightGray')
        # bindメソッドを使うと、そのオブジェクトにイベントを定義できる。
        # 第一引数に<1>を指定すると左クリックした際のイベントとなる
        # 第二引数には呼び出される関数（4行目から定義しているleft_click関数）を記述する
        frame.bind("<1>", left_click)
        frame.bind("<3>", right_click)
        frame.bind("<Enter>", enter)
        # frameにnumアトリビュートを定義する
        frame.num = i
        frame.x = x
        frame.y = y
        # 作成したフレームをフレームのリストに格納する。これでインデックス番号でアクセスすることで
        # 各フレームを操作できる
        frame_list.append(frame)
        # gridを使ってフレームを配置する。packと違いgridを使うと、タテヨコ均等に9列x9列に配置できる
        # rowでヨコ、columnでタテを指定している
        frame.grid(row=y, column=x)
        i += 1

root.mainloop()
