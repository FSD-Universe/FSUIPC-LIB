#  Copyright (c) 2026 Half_nothing
#  SPDX-License-Identifier: MIT
import asyncio
from pathlib import Path
from sys import platform
from typing import Optional

from fsuipc_client import FSUIPCClient


class FSUIPCCmd:
    """FSUIPC 命令行界面"""

    def __init__(self, dll_path: Path):
        self.fsuipc = FSUIPCClient(dll_path)
        self.running = False
        self.auto_refresh = False
        self.refresh_task: Optional[asyncio.Task] = None

    @staticmethod
    def print_banner():
        print("\n" + "=" * 50)
        print("       FSUIPC 客户端命令行界面 v1.0")
        print("=" * 50)
        print("\n输入 'help' 或 'h' 查看可用命令")

    @staticmethod
    def print_help():
        print("""
可用命令:
─────────────────────────────────────────────
  connect / conn      连接到 FSUIPC
  disconnect / disc   断开连接
  status / s          查看连接状态
  version / v         查看 FSUIPC 版本信息
  freq / f            查看当前频率信息
  com1 <khz>          设置 COM1 频率 (例如: com1 122800)
  com2 <khz>          设置 COM2 频率 (例如: com2 122800)
  help / h            显示此帮助信息
  exit / quit / q     退出程序
─────────────────────────────────────────────
""", end="")

    async def print_version(self):
        try:
            res = self.fsuipc.get_fsuipc_version_info()
            if res.request_status:
                print(f"\n✅ FSUIPC 版本信息:")
                print(f"   版本: {res.version}")
                print(f"   模拟器: {res.simulator_name}")
                print(f"   API 版本: {res.api_version}")
            else:
                print(f"\n❌ 获取版本失败: {res.err_message}")
        except Exception as e:
            print(f"\n❌ 错误: {e}")

    async def connect_client(self):
        try:
            res = self.fsuipc.open_fsuipc_client()
            if res.request_status:
                print(f"\n✅ FSUIPC 连接成功:")
                print(f"   版本: {res.version}")
                print(f"   模拟器: {res.simulator_name}")
                print(f"   API 版本: {res.api_version}")
            else:
                print(f"\n❌ 连接失败: {res.err_message}")
        except Exception as e:
            print(f"\n❌ 错误: {e}")

    async def print_status(self):
        try:
            res = self.fsuipc.get_connection_state()
            if res.request_status:
                status_map = {0: "未连接", 1: "已连接"}
                status = status_map.get(res.status, f"未知状态 ({res.status})")
                print(f"\n📊 连接状态: {status}")
            else:
                print(f"\n❌ 获取状态失败: {res.err_message}")
        except Exception as e:
            print(f"\n❌ 错误: {e}")

    async def print_frequency(self):
        """打印频率信息"""
        try:
            freq = self.fsuipc.get_frequency()
            if freq.request_status:
                print(f"\n📻 频率信息:")
                print(f"   频率标志: {bin(freq.frequency_flag)}")
                print(f"   COM1接收: {freq.com1_rx}")
                print(f"   COM2接收: {freq.com2_rx}")

                freqs = freq.frequency

                def format_freq(f):
                    return f"{f / 1000000:.3f} MHz"

                print(f"   COM1 发送: {format_freq(freqs[0])}")
                print(f"   COM1 接收: {format_freq(freqs[1])}")
                print(f"   COM2 发送: {format_freq(freqs[2])}")
                print(f"   COM2 接收: {format_freq(freqs[3])}")
            else:
                print(f"\n❌ 获取频率失败: {freq.err_message}")
        except Exception as e:
            print(f"\n❌ 错误: {e}")

    async def set_com1(self, frequency_khz: int):
        """设置 COM1 频率"""
        try:
            res = self.fsuipc.set_com1_frequency(frequency_khz)
            if res.request_status:
                print(f"\n✅ COM1 已设置为: {frequency_khz / 1000:.3f} MHz")
            else:
                print(f"\n❌ 设置 COM1 失败: {res.err_message}")
        except Exception as e:
            print(f"\n❌ 错误: {e}")

    async def set_com2(self, frequency_khz: int):
        """设置 COM2 频率"""
        try:
            res = self.fsuipc.set_com2_frequency(frequency_khz)
            if res.request_status:
                print(f"\n✅ COM2 已设置为: {frequency_khz / 1000:.3f} MHz")
            else:
                print(f"\n❌ 设置 COM2 失败: {res.err_message}")
        except Exception as e:
            print(f"\n❌ 错误: {e}")

    async def process_command(self, cmd: str):
        cmd = cmd.strip()
        if not cmd:
            return

        parts = cmd.split()
        command = parts[0].lower()
        args = parts[1:]

        # 命令映射
        commands = {
            'help': self.print_help,
            'h': self.print_help,
            'version': self.print_version,
            'v': self.print_version,
            'status': self.print_status,
            's': self.print_status,
            'freq': self.print_frequency,
            'f': self.print_frequency,
            'connect': self.connect_client,
            'conn': self.connect_client,
            'disconnect': self.close_client,
            'disc': self.close_client,
            'exit': self.stop,
            'quit': self.stop,
            'q': self.stop,
        }

        if command in commands:
            if asyncio.iscoroutinefunction(commands[command]):
                await commands[command]()
            else:
                result = commands[command]()
                if asyncio.iscoroutine(result):
                    await result
        elif command == 'com1':
            if len(args) != 1:
                print("❌ 用法: com1 <khz> (例如: com1 122800)")
            else:
                try:
                    freq = int(args[0])
                    await self.set_com1(freq)
                except ValueError:
                    print("❌ 无效的频率值")
        elif command == 'com2':
            if len(args) != 1:
                print("❌ 用法: com2 <khz> (例如: com2 122800)")
            else:
                try:
                    freq = int(args[0])
                    await self.set_com2(freq)
                except ValueError:
                    print("❌ 无效的频率值")
        else:
            print(f"❌ 未知命令: {command} (输入 'help' 查看帮助)")

    def close_client(self):
        try:
            self.fsuipc.close_fsuipc_client()
            print("\n✅ 已断开连接")
        except Exception as e:
            print(f"\n❌ 断开连接错误: {e}")

    def stop(self):
        self.running = False
        self.auto_refresh = False
        if self.refresh_task:
            self.refresh_task.cancel()
        print("\n再见!")

    async def run(self):
        self.running = True
        self.print_banner()

        while self.running:
            try:
                # 非阻塞获取用户输入
                cmd = await asyncio.get_event_loop().run_in_executor(
                    None, input, "\nFSUIPC> "
                )
                await self.process_command(cmd)
            except (KeyboardInterrupt, EOFError):
                self.stop()
                break
            except Exception as e:
                print(f"\n❌ 错误: {e}")


def replace_suffix(path: Path) -> Path:
    match platform:
        case 'win32':
            return path.with_suffix(".dll")
        case 'darwin':
            return path.with_suffix(".dylib")
        case 'linux':
            return path.with_suffix(".so")
        case _:
            raise OSError("unknown platform")


def get_dll_path():
    root = Path(__file__).parent
    dll_path = replace_suffix(root / "libfsuipc")
    if dll_path.exists():
        return dll_path
    build_dll_path = replace_suffix(root.parent / "bin" / "libfsuipc")
    if build_dll_path.exists():
        return build_dll_path
    raise FileNotFoundError("libfsuipc.dll 不存在")


async def main():
    """主函数"""
    dll_path = get_dll_path()
    cmd = FSUIPCCmd(dll_path)
    await cmd.run()


if __name__ == '__main__':
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n程序已退出")
