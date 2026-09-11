'''
解决帮助文档在运行参数是多选一时输出乱换行的问题
'''
import argparse
import shutil
import unicodedata

def CharWidth(Ch):
    """单个字符在终端占用的列数。"""
    if unicodedata.combining(Ch):
        return 0
    return 2 if unicodedata.east_asian_width(Ch) in ("W", "F") else 1


def DisplayWidth(Text):
    return sum(CharWidth(Ch) for Ch in Text)


def WrapByDisplayWidth(Text, Width):
    """按显示宽度换行：英文尽量按空格断词，超长 token（通常是中文）按字符硬切，且保留手动 \\n。"""
    Lines = []
    for Paragraph in Text.split("\n"):
        Paragraph = Paragraph.rstrip()
        if not Paragraph:
            Lines.append("")
            continue

        Line, LineWidth = "", 0
        for Word in Paragraph.split(" "):
            WordWidth = DisplayWidth(Word)
            Space = 1 if Line else 0
            if LineWidth + Space + WordWidth <= Width:
                Line = f"{Line} {Word}" if Line else Word
                LineWidth += Space + WordWidth
                continue

            if Line:
                Lines.append(Line)
                Line, LineWidth = "", 0

            if WordWidth <= Width:
                Line, LineWidth = Word, WordWidth
                continue

            # 单个 token 本身就超宽：逐字符切分
            Buffer, BufferWidth = "", 0
            for Ch in Word:
                if BufferWidth + CharWidth(Ch) > Width:
                    Lines.append(Buffer)
                    Buffer, BufferWidth = "", 0
                Buffer += Ch
                BufferWidth += CharWidth(Ch)
            Line, LineWidth = Buffer, BufferWidth

        if Line:
            Lines.append(Line)

    return Lines or [""]


class InfoGenHelpFormatter(argparse.HelpFormatter):
    """中文友好的帮助格式化器：按显示宽度换行、保留手动换行、额外参数列对齐。"""

    def __init__(self, prog, indent_increment=2, max_help_position=34, width=None):
        columns = shutil.get_terminal_size(fallback=(100, 24)).columns
        width = width or max(min(columns - 2, 120), 60)  # 夹在 60~120 列，避免窄终端/管道下太丑
        super().__init__(prog, indent_increment, max_help_position, width)

    def _split_lines(self, text, width):
        return WrapByDisplayWidth(text, width)

    def _fill_text(self, text, width, indent):
        Available = max(width - DisplayWidth(indent), 10)
        return "\n".join(indent + Line for Line in WrapByDisplayWidth(text, Available))
