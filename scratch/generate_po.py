import json
import os

def main():
    # Load all extracted keys to ensure we have the complete list
    with open("scratch/keys.json", "r", encoding="utf-8") as f:
        keys_dict = json.load(f)
        
    # Add the missing multiline keys
    keys_dict["about_comments"] = (
        "Burn to the Brim (BTTB) is a modern C++20 port of the classic Delphi application designed to optimally fit files and folders onto target storage mediums (CDs, DVDs, Blu-rays, or USBs).\n\n"
        "Features in v4.6.0:\n"
        "- Brand new high-resolution application icon (bttb.ico) and unified website logo (bttb.png)\n"
        "- Minimized search state stack frames & 16MB Win32 stack limit (fixing 0xC00000FD overflows)\n"
        "- Expanded logging buffer limits to 10MB to avoid trace log truncation\n"
        "- Non-blocking incremental GUI rendering & skip file capacity warnings\n"
        "- Offline JSON Index creation and interactive parser\n"
        "- Optional PAR3 parity file generation and verification\n"
        "- Bit-perfect PAR3 copy-based restoration and repair\n"
        "- Theme support including standard dark theme options on Linux GTK4\n"
        "- Improved Estimated Time Left calculation immediately at startup\n"
        "- Named Custom Volume profiles & dynamic Auto Volume sizing\n"
        "- Settings memory restoring the last selected volume configuration\n"
        "- Rule conflict overrides allowing rule-based or semantic grouping to win\n"
        "- Transfer-rate estimated Time Left & status activity spinner\n"
        "- Entropy-Aware Semantic Packing based on MiniLM embeddings\n"
        "- Explorer Context Menu integration & long path support\n\n"
        "BTTB is fully localized and dynamically translates the entire user interface based on standard gettext .po templates in German, Dutch, French, and Spanish.\n\n"
        "Libraries and Attributions Used:\n"
        "- libpar3 (by Yutaka Sawada, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
        "- BLAKE3 (by BLAKE3 team, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
        "- Leopard-RS (by Christopher A. Taylor, BSD 3-Clause): https://github.com/catid/leopard\n"
        "- Galois Field library (by James S. Plank): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
        "Authors: Sander Raaijmakers, Elwin Oost and the Burn to the Brim team"
    )
    
    keys_dict["help_guide_text"] = (
        "1. Directory Split Depth\n"
        "Determines the folder nesting level at which items are split:\n"
        " - Depth 0 (Default): Top-level files and folders are treated as separate items.\n"
        " - Depth 1: Splitting occurs one level deeper, keeping top-level folders intact but splitting their immediate subfolders.\n\n"
        "2. Max Search Time\n"
        "The maximum seconds the backtracking solver is allowed to run. If reached, the best selection found up to that point is used.\n\n"
        "3. Spanning Slack\n"
        "Allows early solver termination once a volume is packed within this number of bytes from the absolute maximum capacity (e.g. 2048 bytes).\n\n"
        "4. File/Folder Grouping Rules\n"
        "Force matching items to remain grouped together on the same volume (e.g., matching '*.mp3' or regex '^album_.*').\n\n"
        "5. Multiple Source Folders (+)\n"
        "Click '+' to specify multiple source folders. BTTB acts as if they are in a single root folder. Nested source paths are ignored.\n\n"
        "6. Create Symbolic Links\n"
        "Instead of copying/moving files to the target folder, BTTB creates lightweight symbolic links pointing back to your original files.\n\n"
        "7. Neural Semantic Packing & MiniLM Setup Guide\n"
        "By specifying a semantic prompt, BTTB groups files with similar content using context-aware deep learning embeddings.\n"
        "To use the preferred, high-accuracy MiniLM neural model, you must install Python 3 and sentence-transformers:\n"
        " - Step 1: Ensure Python 3 & pip are installed.\n"
        "   (Linux: run 'sudo apt install python3 python3-pip python3-venv')\n"
        "   (Windows: Install from https://www.python.org/ and check 'Add Python to PATH')\n"
        " - Step 2: Install sentence-transformers via terminal or command prompt:\n"
        "   Option A (Recommended for simplicity):\n"
        "     pip install sentence-transformers\n"
        "   Option B (Virtual environment isolation):\n"
        "     python3 -m venv bttb_env\n"
        "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
        "     pip install sentence-transformers\n"
        " - Step 3: Restart Burn to the Brim to automatically load MiniLM! If not found, BTTB falls back gracefully to a localized character TF-IDF projector."
    )

    # We will generate translation dictionary translations
    translations = {}
    
    # Define translations for all languages
    # Let's populate translations for Chinese (standard, Mandarin - zh)
    translations["zh"] = {
        "log_start_restore": "正在启动卷恢复/修复...\n",
        "log_volume_path": "卷路径: ",
        "log_recovery_path": "恢复路径: ",
        "log_par3_base": "PAR3 基本名称: ",
        "log_copying_repair": "正在复制卷内容并运行修复。请稍候...\n",
        "log_errors": "日志/错误: ",
        "log_success_restore": "\n成功: 卷已成功复制并在单独的恢复文件夹中修复！\n",
        "log_fail_restore": "\n失败: 恢复或修复失败。\n",
        "log_start_verify": "正在启动卷验证...\n",
        "log_success_verify": "\n成功: 所有文件均已通过验证，完整无缺且完全一致！\n",
        "log_fail_verify_status": "\n验证检测到问题 (状态 ",
        "log_damaged_files": "发现损坏或丢失的文件:\n",
        "log_index_fail": "未发现单个损坏文件，但索引验证失败。PAR3 归档本身可能已损坏。\n",
        "verify_title": "验证与恢复卷...",
        "label_vol_directory": "卷目录:",
        "browse_btn": "浏览...",
        "select_vol_dir_title": "选择卷目录",
        "label_rec_directory": "恢复目录:",
        "select_rec_dir_title": "选择恢复目录",
        "label_par3_base": "PAR3 基本名称:",
        "log_frame_title_verify": "验证与恢复日志",
        "close_btn": "关闭",
        "verify_only_btn": "仅验证",
        "restore_repair_btn": "恢复并修复",
        "iso_title": "创建 ISO 镜像",
        "source_folder": "源文件夹:",
        "target_iso_file": "目标 ISO 文件:",
        "volume_label": "卷标签:",
        "execution_log": "执行日志",
        "generate_btn": "生成",
        "complete_list_src_folders": "源文件夹完整列表:",
        "add_btn": "添加...",
        "edit_btn": "编辑...",
        "remove_btn": "删除",
        "cancel_btn": "取消",
        "ok_btn": "确定",
        "app_title": "Burn to the Brim",
        "app_version": "版本 4.6.0",
        "app_copyright": "版权所有 © 2001-2026 Sander Raaijmakers, Elwin Oost 和 Burn to the Brim 团队",
        "select_medium_label": "选择介质:",
        "auto_size": "自动大小",
        "custom_size": "自定义大小",
        "capacity_label": "容量:",
        "save_cv_label": "保存自定义卷:",
        "custom_vol_name_placeholder": "自定义卷名称",
        "save_btn": "保存",
        "cluster_label": "簇大小 (字节):",
        "slack_label": "容差字节 (Slack):",
        "search_time_label": "最大搜索时间 (秒):",
        "split_depth_label": "目录拆分深度:",
        "skip_empty_chk": "跳过空文件夹 / 文件",
        "skip_unreadable_chk": "跳过无法读取的文件（容错）",
        "integrate_ctx_chk": "集成到 Windows 资源管理器上下文菜单",
        "rule_wins_chk": "规则分组优先:",
        "enable_par3_chk": "启用 PAR3 冗余校验保护:",
        "par3_block_size": "PAR3 块大小 (字节):",
        "par3_redundancy": "PAR3 冗余百分比 (%):",
        "language_label": "语言:",
        "grouping_rules_frame": "文件/文件夹分组规则",
        "pattern_col": "模式",
        "files_col": "文件",
        "folders_col": "文件夹",
        "type_col": "类型",
        "rule_pattern_placeholder": "规则模式 (*.mp3 或 ^[0-9].*\\..*$)",
        "match_files_chk": "匹配文件",
        "match_folders_chk": "匹配文件夹",
        "regex_pattern_chk": "正则表达式模式",
        "add_rule_btn": "添加规则",
        "remove_selected_btn": "删除所选",
        "restart_notice": "语言首选项已保存。请重新启动 Burn to the Brim 以应用更改。",
        "restart_title": "语言已更改",
        "dir_setup_group": "目录设置",
        "target_folder": "目标文件夹:",
        "semantic_prompt": "语义提示:",
        "move_files_chk": "将装配好的文件夹/文件移动到目标文件夹",
        "create_symlinks_chk": "在目标文件夹中创建符号链接（默认）",
        "span_chk": "跨多个卷进行划分 (Volume_1, Volume_2 等)",
        "trace_chk": "启用详细的求解器诊断跟踪 (Trace)",
        "progress_frame_title": "装配好的介质容量",
        "filled_label": "已填充: 0.00%",
        "log_frame_title": "状态和求解器日志",
        "time_left_label": "剩余时间: --:--",
        "test_btn": "测试",
        "start_btn": "开始",
        "stop_btn": "停止",
        "pref_title": "首选项",
        "create_iso_btn": "创建 ISO...",
        "help_btn": "帮助",
        "about_btn": "关于...",
        "import_json_btn": "导入 JSON...",
        "verify_restore_btn": "验证并恢复...",
        "tooltip_import_json": "导入并解析离线 JSON 索引文件以查看求解好的卷文件详细信息。",
        "tooltip_verify_restore": "使用 PAR3 校验文件验证求解好的卷或复制并恢复损坏的文件。",
        "tooltip_test": "模拟装配而不执行实际磁盘文件操作，以测试卷空间利用率。",
        "tooltip_start": "运行求解器并根据首选项组织文件/文件夹。",
        "tooltip_stop": "中止当前正在运行的装配或文件组织过程。",
        "tooltip_create_iso": "从求解好的卷中创建 ISO 文件系统镜像。",
        "tooltip_source": "用于打包和合并的文件夹路径（分号分隔）。",
        "tooltip_target": "组织和写入卷的目标路径。",
        "tooltip_semantic": "输入提示词如 'keep audio together'，以通过 MiniLM 分析文件。",
        "tooltip_move": "直接将原始文件/文件夹移动/剪切到目标卷文件夹中。",
        "tooltip_symlink": "在目标卷文件夹中创建文件系统符号链接（非破坏性）。",
        "tooltip_span": "允许将内容跨越到多个顺序命名的文件夹中。",
        "tooltip_trace": "在日志中显示额外的详细诊断和性能指标。",
        "manage_src_folders_title": "管理源文件夹",
        "help_guide_title": "Burn to the Brim (BTTB) 帮助指南",
        "help_title": "帮助 - Burn to the Brim",
        "about_title": "关于 Burn to the Brim",
        "media_size_label": "介质大小:",
        "save_cv_tooltip": "将当前容量、扇区和容差保存为命名的自定义卷",
        "skip_empty_label": "跳过空文件/目录:",
        "dark_theme_chk": "启用深色主题:",
        "add_src_dir_title": "添加源目录",
        "edit_src_dir_title": "编辑源目录",
        "start_tooltip": "开始将文件组织到最佳的卷中",
        "test_tooltip": "在不修改磁盘文件的情况下模拟组织文件并计算指标",
        "stop_tooltip": "停止当前的求解器或复制操作",
        "rel_path_col": "相对路径 / 类别",
        "size_col": "大小",
        "fitted_status_col": "装配状态",
        "source_tooltip": "包含要组织文件的源目录",
        "browse_src_tooltip": "浏览源目录",
        "target_tooltip": "将创建已组织卷的目标目录",
        "browse_dest_tooltip": "浏览目标目录",
        "select_dest_dir_title": "选择目标目录",
        "semantic_placeholder": "例如: 保持相似内容在一起",
        "semantic_tooltip": "指定语义描述，使用 MiniLM AI 对相似文件进行聚类",
        "move_tooltip": "将文件移动到其目标卷，而不是复制或创建符号链接",
        "symlink_tooltip": "在目标位置创建文件的符号链接，而不是复制它们",
        "span_tooltip": "将剩余文件顺序装配到多个卷中，而不仅仅是第一个卷",
        "trace_tooltip": "在日志中显示额外的详细诊断和性能指标",
        "capacity_recommend_prompt_gtk": "最大扫描项需要至少 %.2f MB 的卷容量。\n\n选择一个操作:",
        "btn_resize": "调整大小",
        "btn_skip": "跳过文件",
        "btn_cancel": "取消",
        "log_warn_dir_unreadable_1": "警告: 目录 '",
        "log_warn_dir_unreadable_2": "' 无法读取 (",
        "log_scan_aborted_unreadable_dir": "由于目录无法读取，用户中止了扫描。",
        "log_warn_item_unreadable_1": "警告: 文件或文件夹于 '",
        "log_warn_item_unreadable_2": "' 无法读取 (",
        "log_scan_aborted_unreadable_item": "由于项目无法读取，用户中止了扫描。",
        "log_warn_failed_iterate_dir_1": "警告: 无法遍历目录 '",
        "log_warn_failed_iterate_dir_2": "' (",
        "log_scan_aborted_failed_iterate": "由于目录遍历失败，用户中止了扫描。",
        "log_scanning_folder": "正在扫描文件夹: ",
        "log_found_items": "找到可装配的项目: ",
        "log_search_time_limit_exceeded_1": "已超出搜索时间限制 (",
        "log_search_time_limit_exceeded_2": " 秒)。",
        "log_new_best_space_utilization": "新的最佳空间利用率: ",
        "log_selection_within_slack_1": "找到容差范围内的选择 (",
        "log_selection_within_slack_2": "%)。提前终止搜索。",
        "log_no_items_to_fit": "没有要装配的项目。",
        "log_auto_volume_cap_set_1": "自动卷容量设置为: ",
        "log_auto_volume_cap_set_2": " 字节 (",
        "log_warn_item_exceeds_cap_1": "警告: 项目 '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " 字节) 大于目标卷容量 (",
        "log_capacity_adapted_1": "容量已调整为 ",
        "log_capacity_adapted_2": " 字节。正在重试求解器...",
        "log_capacity_skip_larger": "已跳过较大的文件。正在重试求解器...",
        "log_solver_aborted_files_exceed_cap": "求解器已中止: 数据集包含超出容量的文件。",
        "log_solver_aborted_files_exceed_cap_adapt": "求解器已中止: 数据集包含超出容量的文件。请调整容量或启用交互式重试。",
        "log_volume_packing_estimation_header": "--- 卷装配预估 ---",
        "log_total_dataset_size_1": "数据集总大小: ",
        "log_total_dataset_size_2": " 字节 (",
        "log_target_volume_cap_1": "目标卷容量: ",
        "log_target_volume_cap_2": " 字节 (",
        "log_theoretical_min_vols": "理论上所需的最小卷数: ",
        "log_medium_capacity_1": "介质容量: ",
        "log_medium_capacity_2": " 扇区 (",
        "log_solving_for_vol_header_1": "\r\n--- 正在求解卷 ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "正在求解最佳箱装配选择...",
        "log_finished_solving_1": "求解完成。用时: ",
        "log_finished_solving_2": " 毫秒",
        "log_single_vol_sol_forcing": "单卷解决方案: 强制选择所有文件以保证 100% 包含...",
        "log_backtracking_failed_fallback_greedy": "回溯求解器未找到装配（或超时）。正在退回到贪婪分配以确保包含所有剩余文件...",
        "log_forcing_progress_exceeds_cap_1": "强制进度: 正在装配大于介质容量的剩余项 (",
        "log_optimal_selection_covers": "最佳选择覆盖: ",
        "log_selected_items_for_volume_1": "卷已选项目 ",
        "log_selected_items": "已选项目:",
        "log_bytes_suffix": " 字节)",
        "log_organizing_item": "正在组织项目: ",
        "log_failed_symlink_retry_copy_1": "无法为 ",
        "log_failed_symlink_retry_copy_2": " 创建符号链接 (正在重试复制)",
        "log_failed_organize_item_1": "无法组织 ",
        "log_generating_par3_for_volume_1": "正在为卷生成 PAR3 校验文件 ",
        "log_success_par3_for_volume_1": "成功为卷创建了 PAR3 冗余保护 ",
        "log_failed_par3_creation": "PAR3 创建失败: ",
        "log_test_simulation_complete": "--- 测试模拟完成 ---",
        "log_no_files_written_on_disk": "磁盘上未写入、复制、移动或创建任何文件的符号链接。",
        "log_completed_file_organization": "文件组织已完成。",
        "log_created_offline_json_index": "已创建离线 JSON 索引文件: ",
        "log_failed_create_json_index": "无法创建 JSON 索引文件: ",
        "log_generating_semantic_embeddings": "正在生成语义嵌入...",
        "log_failed_write_temp_embed_input": "无法写入临时嵌入输入文件: ",
        "log_warn_python_embed_engine_failed": "警告: 子进程 python 嵌入引擎失败或未安装 python3。",
        "log_semantic_grouping_fallback": "-> 语义分组将退回到本地字符串匹配度量。",
        "log_failed_read_temp_embed_output": "无法读取临时嵌入输出文件: ",
        "log_warn_embedded_output_empty": "警告: 嵌入输出为空。",
        "log_running_semantic_clustering": "正在运行凝聚层语义聚类 (阈值=0.6)...",
        "log_semantic_clustering_completed": "语义聚类完成。总合并组数: ",
        "about_comments": (
            "Burn to the Brim (BTTB) 是经典 Delphi 应用程序的现代 C++20 移植版本，旨在将文件和文件夹优化装配到目标存储介质（CD、DVD、蓝光或 USB 闪存盘）上。\n\n"
            "v4.6.0 中的功能:\n"
            "- 全新的高分辨率应用程序图标 (bttb.ico) 和统一的网站标志 (bttb.png)\n"
            "- 最小化搜索状态栈帧和 16MB Win32 栈限制（修复了 0xC00000FD 栈溢出）\n"
            "- 将日志缓冲区限制扩大到 10MB，以避免跟踪日志被截断\n"
            "- 非阻塞渐进式 GUI 渲染和跳过文件容量警告\n"
            "- 离线 JSON 索引创建和交互式解析器\n"
            "- 可选的 PAR3 冗余校验文件生成和验证\n"
            "- 基于 PAR3 复制的完全位元级数据恢复和修复\n"
            "- 主题支持，包括 Linux GTK4 上的标准深色主题选项\n"
            "- 启动时立即改进的预计剩余时间计算\n"
            "- 命名的自定义卷配置文件和动态自动卷大小调整\n"
            "- 设置记忆，恢复上次选择的卷配置\n"
            "- 规则冲突覆盖，允许基于规则的分组或语义分组优先\n"
            "- 基于传输速率估计的剩余时间与状态活动旋转指示器\n"
            "- 基于 MiniLM 嵌入的熵感知语义打包\n"
            "- Windows 资源管理器右键菜单集成和长路径支持\n\n"
            "BTTB 已完全本地化，基于德语、荷兰语、法语和西班牙语中的标准 gettext .po 模板动态翻译整个用户界面。\n\n"
            "所使用的库和归属:\n"
            "- libpar3（由 Yutaka Sawada 开发，LGPL v2.1+）: https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3（由 BLAKE3 团队开发，CC0/Apache-2.0）: https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS（由 Christopher A. Taylor 开发，BSD 3-Clause）: https://github.com/catid/leopard\n"
            "- Galois Field 库（由 James S. Plank 开发）: http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "作者: Sander Raaijmakers, Elwin Oost 和 Burn to the Brim 团队"
        ),
        "help_guide_text": (
            "1. 目录拆分深度\n"
            "确定拆分项的文件夹嵌套级别:\n"
            " - 深度 0（默认）: 顶级文件和文件夹被视为单独的项。\n"
            " - 深度 1: 拆分发生在深一级，保持顶级文件夹完好无损，但拆分其直接子文件夹。\n\n"
            "2. 最大搜索时间\n"
            "允许回溯求解器运行的最大秒数。如果达到此限制，将使用在此之前找到的最佳选择。\n\n"
            "3. 容差字节 (Spanning Slack)\n"
            "一旦卷填充到距离绝对最大容量此字节数以内（例如 2048 字节），允许求解器提前终止。\n\n"
            "4. 文件/文件夹分组规则\n"
            "强制匹配项在同一卷上保持分组在一起（例如，匹配 '*.mp3' 或正则表达式 '^album_.*'）。\n\n"
            "5. 多个源文件夹 (+)\n"
            "点击 '+' 指定多个源文件夹。BTTB 的行为就像它们在单个根文件夹中一样。嵌套的源路径将被忽略。\n\n"
            "6. 创建符号链接\n"
            "BTTB 不会将文件复制/移动到目标文件夹，而是创建指向原始文件的轻量级符号链接。\n\n"
            "7. 神经语义打包与 MiniLM 设置指南\n"
            "通过指定语义提示词，BTTB 使用上下文感知的深度学习嵌入对内容相似的文件进行分组。\n"
            "要使用首选的高精度 MiniLM 神经模型，您必须安装 Python 3 和 sentence-transformers:\n"
            " - 步骤 1: 确保安装了 Python 3 和 pip。\n"
            "   (Linux: 运行 'sudo apt install python3 python3-pip python3-venv')\n"
            "   (Windows: 从 https://www.python.org/ 安装并勾选 'Add Python to PATH')\n"
            " - 步骤 2: 通过终端或命令提示符安装 sentence-transformers:\n"
            "   选项 A (推荐，因简单易行):\n"
            "     pip install sentence-transformers\n"
            "   选项 B (使用虚拟环境隔离):\n"
            "     python3 -m venv bttb_env\n"
            "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
            "     pip install sentence-transformers\n"
            " - 步骤 3: 重新启动 Burn to the Brim 以自动加载 MiniLM！如果未找到，BTTB 将优雅地退回到本地化的字符 TF-IDF 项目器。"
        )
    }
    
    # Let's populate translations for Japanese (ja)
    translations["ja"] = {
        "log_start_restore": "ボリュームの復元/修復を開始しています...\n",
        "log_volume_path": "ボリュームパス: ",
        "log_recovery_path": "リカバリパス: ",
        "log_par3_base": "PAR3ベース名: ",
        "log_copying_repair": "ボリュームの内容をコピーし、修復を実行しています。お待ちください...\n",
        "log_errors": "ログ/エラー: ",
        "log_success_restore": "\n成功: ボリュームのコピーと修復が別のリカバリフォルダーに正常に完了しました！\n",
        "log_fail_restore": "\n失敗: 復元または修復に失敗しました。\n",
        "log_start_verify": "ボリュームの検証を開始しています...\n",
        "log_success_verify": "\n成功: すべてのファイルが検証され、正常（ビットパーフェクト）です！\n",
        "log_fail_verify_status": "\n検証中に問題が検出されました (ステータス ",
        "log_damaged_files": "破損または不足しているファイルが見つかりました:\n",
        "log_index_fail": "個別の破損ファイルは特定されませんでしたが、インデックスの検証に失敗しました。PAR3アーカイブ自体が破損している可能性があります。\n",
        "verify_title": "ボリュームの検証と修復...",
        "label_vol_directory": "ボリュームディレクトリ:",
        "browse_btn": "参照...",
        "select_vol_dir_title": "ボリュームディレクトリの選択",
        "label_rec_directory": "リカバリディレクトリ:",
        "select_rec_dir_title": "リカバリディレクトリの選択",
        "label_par3_base": "PAR3ベース名:",
        "log_frame_title_verify": "検証および復元ログ",
        "close_btn": "閉じる",
        "verify_only_btn": "検証のみ",
        "restore_repair_btn": "復元と修復",
        "iso_title": "ISOイメージの作成",
        "source_folder": "コピー元フォルダー:",
        "target_iso_file": "対象ISOファイル:",
        "volume_label": "ボリュームラベル:",
        "execution_log": "実行ログ",
        "generate_btn": "生成",
        "complete_list_src_folders": "コピー元フォルダーの完全なリスト:",
        "add_btn": "追加...",
        "edit_btn": "編集...",
        "remove_btn": "削除",
        "cancel_btn": "キャンセル",
        "ok_btn": "OK",
        "app_title": "Burn to the Brim",
        "app_version": "バージョン 4.6.0",
        "app_copyright": "Copyright © 2001-2026 Sander Raaijmakers, Elwin Oost and the Burn to the Brim team",
        "select_medium_label": "メディアの選択:",
        "auto_size": "自動サイズ",
        "custom_size": "カスタムサイズ",
        "capacity_label": "容量:",
        "save_cv_label": "カスタムボリュームの保存:",
        "custom_vol_name_placeholder": "カスタムボリューム名",
        "save_btn": "保存",
        "cluster_label": "クラスターサイズ (バイト):",
        "slack_label": "スラックバイト数 (Slack):",
        "search_time_label": "最大検索時間 (秒):",
        "split_depth_label": "ディレクトリ分割の深さ:",
        "skip_empty_chk": "空のフォルダー/ファイルをスキップする",
        "skip_unreadable_chk": "読み取り不可能なファイルをスキップする (緩やか)",
        "integrate_ctx_chk": "Windows エクスプローラーのコンテキストメニューに統合する",
        "rule_wins_chk": "ルールベースのグループ化を優先する:",
        "enable_par3_chk": "PAR3パリティ保護を有効にする:",
        "par3_block_size": "PAR3ブロックサイズ (バイト):",
        "par3_redundancy": "PAR3冗長率 (%):",
        "language_label": "言語:",
        "grouping_rules_frame": "ファイル/フォルダーのグループ化ルール",
        "pattern_col": "パターン",
        "files_col": "ファイル",
        "folders_col": "フォルダー",
        "type_col": "タイプ",
        "rule_pattern_placeholder": "ルールパターン (*.mp3 または ^[0-9].*\\..*$)",
        "match_files_chk": "ファイルを一致させる",
        "match_folders_chk": "フォルダーを一致させる",
        "regex_pattern_chk": "正規表現パターン",
        "add_rule_btn": "ルールを追加",
        "remove_selected_btn": "選択した項目を削除",
        "restart_notice": "言語設定が保存されました。変更を適用するには、Burn to the Brim を再起動してください。",
        "restart_title": "言語の変更",
        "dir_setup_group": "ディレクトリ設定",
        "target_folder": "コピー先フォルダー:",
        "semantic_prompt": "セマンティックプロンプト:",
        "move_files_chk": "配置されたフォルダー/ファイルをコピー先フォルダーに移動する",
        "create_symlinks_chk": "コピー先フォルダーにシンボリックリンクを作成する (デフォルト)",
        "span_chk": "複数のボリュームに分割する (Volume_1, Volume_2 など)",
        "trace_chk": "詳細なソルバー診断トレースを有効にする (Trace)",
        "progress_frame_title": "配置されたメディアの容量",
        "filled_label": "使用率: 0.00%",
        "log_frame_title": "ステータスおよびソルバーログ",
        "time_left_label": "残り時間: --:--",
        "test_btn": "テスト",
        "start_btn": "開始",
        "stop_btn": "停止",
        "pref_title": "設定",
        "create_iso_btn": "ISOの作成...",
        "help_btn": "ヘルプ",
        "about_btn": "情報...",
        "import_json_btn": "JSONのインポート...",
        "verify_restore_btn": "検証と復元...",
        "tooltip_import_json": "オフラインの JSON インデックスファイルをインポートして解析し、解決されたボリュームファイルのファイル詳細を表示します。",
        "tooltip_verify_restore": "PAR3 アーカイブを使用して、解決されたボリュームを検証するか、破損したファイルをコピーして復元します。",
        "tooltip_test": "ディスク上のファイルを変更せずにパッキングをシミュレートし、ボリュームの使用状況をテストします。",
        "tooltip_start": "ソルバーを実行し、設定に従ってファイル/フォルダーを整理します。",
        "tooltip_stop": "現在実行中のパッキングまたはファイル整理プロセスを中止します。",
        "tooltip_create_iso": "解決されたボリュームから ISO ファイルシステムイメージを作成します。",
        "tooltip_source": "パックして統合するフォルダーのセミコロン区切りのパス。",
        "tooltip_target": "整理されたボリュームが作成されるコピー先パス。",
        "tooltip_semantic": "MiniLM を使用してファイルを分析するために、「音楽をまとめる」などのプロンプトを入力します。",
        "tooltip_move": "元のファイル/フォルダーをコピー先ボリュームフォルダーに直接移動/切り取りします。",
        "tooltip_symlink": "コピー先ボリュームフォルダーにファイルシステムのシンボリックリンクを作成します（非破壊的）。",
        "tooltip_span": "連番で名前が付けられた複数のフォルダーにコンテンツを分割できるようにします。",
        "tooltip_trace": "フォルダー/ファイルがどのように選択および分割されるかを示す詳細な診断ログを表示します。",
        "manage_src_folders_title": "コピー元フォルダーの管理",
        "help_guide_title": "Burn to the Brim (BTTB) ヘルプガイド",
        "help_title": "ヘルプ - Burn to the Brim",
        "about_title": "Burn to the Brim について",
        "media_size_label": "メディアサイズ:",
        "save_cv_tooltip": "現在の容量、セクター、およびスラックを名前付きのカスタムボリュームとして保存します",
        "skip_empty_label": "空のファイル/ディレクトリをスキップ:",
        "dark_theme_chk": "ダークテーマを有効にする:",
        "add_src_dir_title": "コピー元ディレクトリの追加",
        "edit_src_dir_title": "コピー元ディレクトリの編集",
        "start_tooltip": "ファイルを最適なボリュームに整理し始めます",
        "test_tooltip": "ディスク上のファイルを変更せずにファイルの整理をシミュレートし、メトリックを計算します",
        "stop_tooltip": "現在のソルバーまたはコピー操作を停止します",
        "rel_path_col": "相対パス / カテゴリ",
        "size_col": "サイズ",
        "fitted_status_col": "配置ステータス",
        "source_tooltip": "整理するファイルを含むコピー元ディレクトリ",
        "browse_src_tooltip": "コピー元ディレクトリを参照",
        "target_tooltip": "整理されたボリュームが作成されるコピー先ディレクトリ",
        "browse_dest_tooltip": "コピー先ディレクトリを参照",
        "select_dest_dir_title": "コピー先ディレクトリの選択",
        "semantic_placeholder": "例: 似たコンテンツをまとめる",
        "semantic_tooltip": "MiniLM AI を使用して類似ファイルをクラスタリングするためのセマンティック説明を指定します",
        "move_tooltip": "コピーやシンボリックリンクを作成する代わりに、ファイルをコピー先ボリュームに移動します",
        "symlink_tooltip": "ファイルをコピーする代わりに、コピー先にシンボリックリンクを作成します",
        "span_tooltip": "最初のボリュームだけでなく、残りのファイルを複数のボリュームに順次配置します",
        "trace_tooltip": "ログに詳細な診断とパフォーマンス指標を表示します",
        "capacity_recommend_prompt_gtk": "スキャンされた最大のアイテムには、少なくとも %.2f MB のボリューム容量が必要です。\n\nアクションを選択してください:",
        "btn_resize": "サイズ変更",
        "btn_skip": "ファイルをスキップ",
        "btn_cancel": "キャンセル",
        "log_warn_dir_unreadable_1": "警告: ディレクトリ '",
        "log_warn_dir_unreadable_2": "' は読み取り不可能です (",
        "log_scan_aborted_unreadable_dir": "読み取り不可能なディレクトリのため、ユーザーによってスキャンが中止されました。",
        "log_warn_item_unreadable_1": "警告: '",
        "log_warn_item_unreadable_2": "' のファイルまたはフォルダーは読み取り不可能です (",
        "log_scan_aborted_unreadable_item": "読み取り不可能なアイテムのため、ユーザーによってスキャンが中止されました。",
        "log_warn_failed_iterate_dir_1": "警告: ディレクトリ '",
        "log_warn_failed_iterate_dir_2": "' の反復処理に失敗しました (",
        "log_scan_aborted_failed_iterate": "ディレクトリ反復処理の失敗のため、ユーザーによってスキャンが中止されました。",
        "log_scanning_folder": "フォルダーをスキャン中: ",
        "log_found_items": "配置するアイテムが見つかりました: ",
        "log_search_time_limit_exceeded_1": "検索時間制限を超過しました (",
        "log_search_time_limit_exceeded_2": " 秒)。",
        "log_new_best_space_utilization": "新しい最適なスペース利用率: ",
        "log_selection_within_slack_1": "スラック許容範囲内の選択が見つかりました (",
        "log_selection_within_slack_2": "%)。検索を早期終了します。",
        "log_no_items_to_fit": "配置するアイテムはありません。",
        "log_auto_volume_cap_set_1": "自動ボリューム容量が設定されました: ",
        "log_auto_volume_cap_set_2": " バイト (",
        "log_warn_item_exceeds_cap_1": "警告: アイテム '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " バイト) は対象のボリュームサイズよりも大きいです (",
        "log_capacity_adapted_1": "容量が ",
        "log_capacity_adapted_2": " バイトに適応されました。ソルバーを再試行しています...",
        "log_capacity_skip_larger": "大きいファイルがスキップされました。ソルバーを再試行しています...",
        "log_solver_aborted_files_exceed_cap": "ソルバーが中止されました: データセットに容量を超えるファイルが含まれています。",
        "log_solver_aborted_files_exceed_cap_adapt": "ソルバーが中止されました: データセットに容量を超えるファイルが含まれています。容量を調整するか、インタラクティブな再試行を有効にしてください。",
        "log_volume_packing_estimation_header": "--- ボリュームパッキングの見積もり ---",
        "log_total_dataset_size_1": "データセットの総サイズ: ",
        "log_total_dataset_size_2": " バイト (",
        "log_target_volume_cap_1": "対象ボリューム容量: ",
        "log_target_volume_cap_2": " バイト (",
        "log_theoretical_min_vols": "必要な理論上の最小ボリューム数: ",
        "log_medium_capacity_1": "メディア容量: ",
        "log_medium_capacity_2": " セクター (",
        "log_solving_for_vol_header_1": "\r\n--- ボリュームの解決中 ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "最適なビン選択を解決しています...",
        "log_finished_solving_1": "解決が完了しました。経過時間: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "単一ボリュームのソリューション: 100% の包含を保証するためにすべてのファイルの選択を強制しています...",
        "log_backtracking_failed_fallback_greedy": "バックトラッキングソルバーが適合を見つけられませんでした（またはタイムアウトしました）。残りのすべてのファイルが含まれるように、貪欲法のアロケーションにフォールバックしています...",
        "log_forcing_progress_exceeds_cap_1": "強制進行: メディア容量より大きい残りのアイテムをパッキングしています (",
        "log_optimal_selection_covers": "最適な選択の範囲: ",
        "log_selected_items_for_volume_1": "ボリュームに選択されたアイテム ",
        "log_selected_items": "選択されたアイテム:",
        "log_bytes_suffix": " バイト)",
        "log_organizing_item": "アイテムを整理中: ",
        "log_failed_symlink_retry_copy_1": "シンボリックリンクの作成に失敗しました: ",
        "log_failed_symlink_retry_copy_2": " (コピーで再試行中)",
        "log_failed_organize_item_1": "整理に失敗しました: ",
        "log_generating_par3_for_volume_1": "ボリュームの PAR3 パリティファイルを生成中 ",
        "log_success_par3_for_volume_1": "ボリュームの PAR3 パリティ保護が正常に作成されました ",
        "log_failed_par3_creation": "PAR3 の作成に失敗しました: ",
        "log_test_simulation_complete": "--- テストシミュレーション完了 ---",
        "log_no_files_written_on_disk": "ディスク上へのファイルのコピー、シンボリックリンク作成、または移動は行われませんでした。",
        "log_completed_file_organization": "ファイルの整理が完了しました。",
        "log_created_offline_json_index": "オフラインの JSON インデックスファイルが作成されました: ",
        "log_failed_create_json_index": "JSON インデックスファイルの作成に失敗しました: ",
        "log_generating_semantic_embeddings": "セマンティック埋め込みを生成中...",
        "log_failed_write_temp_embed_input": "一時的な埋め込み入力ファイルの書き込みに失敗しました: ",
        "log_warn_python_embed_engine_failed": "警告: サブプロセスの python 埋め込みエンジンが失敗したか、python3 がインストールされていません。",
        "log_semantic_grouping_fallback": "-> セマンティックグループ化はローカルの文字列一致メトリックにフォールバックします。",
        "log_failed_read_temp_embed_output": "一時的な埋め込み出力ファイルの読み込みに失敗しました: ",
        "log_warn_embedded_output_empty": "警告: 埋め込み出力が空です。",
        "log_running_semantic_clustering": "凝集型セマンティッククラスタリングを実行中 (しきい値=0.6)...",
        "log_semantic_clustering_completed": "セマンティッククラスタリングが完了しました。統合されたグループの総数: ",
        "about_comments": (
            "Burn to the Brim (BTTB) は、ファイルやフォルダーを対象の記憶媒体（CD、DVD、Blu-ray、またはUSB）に最適に配置するために設計された、クラシックなDelphiアプリケーションの最新のC++20移植版です。\n\n"
            "v4.6.0の機能:\n"
            "- まったく新しい高解像度アプリケーションアイコン（bttb.ico）と統一されたウェブサイトロゴ（bttb.png）\n"
            "- 検索状態スタックフレームの最小化と16MBのWin32スタック制限（0xC00000FDオーバーフローの修正）\n"
            "- トレースログの切り捨てを回避するためのログバッファ制限を10MBに拡張\n"
            "- 非ブロッキングのインクリメンタルGUIレンダリングとファイル容量警告のスキップ\n"
            "- オフラインJSONインデックスの作成とインタラクティブなパーサー\n"
            "- オプションのPAR3パリティファイルの生成と検証\n"
            "- ビットパーフェクトなPAR3コピーベースの復元と修復\n"
            "- Linux GTK4での標準のダークテーマオプションを含むテーマサポート\n"
            "- 起動時に即座に行われる改善された残り時間推定計算\n"
            "- 名前付きカスタムボリュームプロファイルと動的な自動ボリュームサイズ調整\n"
            "- 最後に選択したボリューム構成を復元する設定メモリ\n"
            "- ルールベースまたはセマンティックのグループ化が優先されるルール競合のオーバーライド\n"
            "- 転送速度推定による残り時間表示とステータスアクティビティスピナー\n"
            "- MiniLM埋め込みに基づくエントロピー認識セマンティックパッキング\n"
            "- エクスプローラーのコンテキストメニューの統合と長いパスのサポート\n\n"
            "BTTBは完全にローカライズされており、ドイツ語、オランダ語、フランス語、スペイン語の標準のgettext .poテンプレートに基づいて、ユーザーインターフェース全体を動的に翻訳します。\n\n"
            "使用されているライブラリと謝辞:\n"
            "- libpar3 (澤田豊氏作, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3 (BLAKE3チーム作, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS (Christopher A. Taylor氏作, BSD 3-Clause): https://github.com/catid/leopard\n"
            "- Galois Fieldライブラリ (James S. Plank氏作): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "著者: Sander Raaijmakers, Elwin Oost と Burn to the Brim チーム"
        ),
        "help_guide_text": (
            "1. ディレクトリ分割の深さ\n"
            "アイテムが分割されるフォルダーのネストレベルを決定します:\n"
            " - 深さ 0 (デフォルト): 最上位のファイルとフォルダーは個別のアイテムとして扱われます。\n"
            " - 深さ 1: 分割は1レベル深く実行され、最上位のフォルダーはそのまま維持されますが、その直接のサブフォルダーが分割されます。\n\n"
            "2. 最大検索時間\n"
            "バックトラッキングソルバーの実行が許可される最大秒数。制限に達した場合、その時点までに見つかった最良の選択が使用されます。\n\n"
            "3. スラックバイト数 (Spanning Slack)\n"
            "ボリュームが絶対最大容量からこのバイト数以内（例：2048バイト）にパッキングされると、ソルバーの早期終了を許可します。\n\n"
            "4. ファイル/フォルダーのグループ化ルール\n"
            "一致するアイテムが同じボリューム上にグループ化されたままになるように強制します（例：'*.mp3' または正規表現 '^album_.*' に一致）。\n\n"
            "5. 複数のコピー元フォルダー (+)\n"
            "'+' をクリックして、複数のコピー元フォルダーを指定します。BTTB はそれらが単一のルートフォルダーにあるかのように動作します。ネストされたコピー元パスは無視されます。\n\n"
            "6. シンボリックリンクの作成\n"
            "ファイルをコピー先フォルダーにコピー/移動する代わりに、BTTB は元のファイルを指す軽量なシンボリックリンクを作成します。\n\n"
            "7. ニューラルセマンティックパッキングと MiniLM セットアップガイド\n"
            "セマンティックプロンプトを指定することで、BTTB は文脈認識ディープラーニング埋め込みを使用して、類似したコンテンツを持つファイルをグループ化します。\n"
            "推奨される高精度 MiniLM ニューラルモデルを使用するには、Python 3 と sentence-transformers をインストールする必要があります:\n"
            " - ステップ 1: Python 3 と pip がインストールされていることを確認します。\n"
            "   (Linux: 'sudo apt install python3 python3-pip python3-venv' を実行)\n"
            "   (Windows: https://www.python.org/ からインストールし、「Add Python to PATH」にチェックを入れる)\n"
            " - ステップ 2: ターミナルまたはコマンドプロンプトを介して sentence-transformers をインストールします:\n"
            "   オプション A (簡単でおすすめ):\n"
            "     pip install sentence-transformers\n"
            "   オプション B (仮想環境による分離):\n"
            "     python3 -m venv bttb_env\n"
            "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
            "     pip install sentence-transformers\n"
            " - ステップ 3: Burn to the Brim を再起動して、MiniLM を自動的に読み込みます！見つからない場合、BTTB はローカライズされた文字 TF-IDF プロジェクターに適切にフォールバックします。"
        )
    }

    # Now let's define Italian (it)
    translations["it"] = {
        "log_start_restore": "Avvio del ripristino/riparazione del volume...\n",
        "log_volume_path": "Percorso del volume: ",
        "log_recovery_path": "Percorso di ripristino: ",
        "log_par3_base": "Nome base PAR3: ",
        "log_copying_repair": "Copia dei contenuti del volume ed esecuzione della riparazione. Attendere...\n",
        "log_errors": "Log/Errori: ",
        "log_success_restore": "\nSUCCESSO: Volume copiato e riparato con successo nella cartella di ripristino separata!\n",
        "log_fail_restore": "\nFALLIMENTO: Ripristino o riparazione falliti.\n",
        "log_start_verify": "Avvio della verifica del volume...\n",
        "log_success_verify": "\nSUCCESSO: Tutti i file sono verificati e sono puliti/bit-perfect!\n",
        "log_fail_verify_status": "\nLa verifica ha rilevato problemi (stato ",
        "log_damaged_files": "File danneggiati o mancanti trovati:\n",
        "log_index_fail": "Nessun singolo file danneggiato identificato, ma la verifica dell'indice è fallita. L'archivio PAR3 stesso potrebbe essere danneggiato.\n",
        "verify_title": "Verifica & Ripristina Volumi...",
        "label_vol_directory": "Directory del volume:",
        "browse_btn": "Sfoglia...",
        "select_vol_dir_title": "Seleziona directory del volume",
        "label_rec_directory": "Directory di ripristino:",
        "select_rec_dir_title": "Seleziona directory di ripristino",
        "label_par3_base": "Nome base PAR3:",
        "log_frame_title_verify": "Log di verifica e ripristino",
        "close_btn": "Chiudi",
        "verify_only_btn": "Solo Verifica",
        "restore_repair_btn": "Ripristina & Ripara",
        "iso_title": "Crea Immagine ISO",
        "source_folder": "Cartella di origine:",
        "target_iso_file": "File ISO di destinazione:",
        "volume_label": "Etichetta del volume:",
        "execution_log": "Log di esecuzione",
        "generate_btn": "Genera",
        "complete_list_src_folders": "Elenco completo delle cartelle di origine:",
        "add_btn": "Aggiungi...",
        "edit_btn": "Modifica...",
        "remove_btn": "Rimuovi",
        "cancel_btn": "Annulla",
        "ok_btn": "OK",
        "app_title": "Burn to the Brim",
        "app_version": "Versione 4.6.0",
        "app_copyright": "Copyright © 2001-2026 Sander Raaijmakers, Elwin Oost e il team di Burn to the Brim",
        "select_medium_label": "Seleziona Supporto:",
        "auto_size": "Dimensione Automatica",
        "custom_size": "Dimensione Personalizzata",
        "capacity_label": "Capacità:",
        "save_cv_label": "Salva Volume Pers.:",
        "custom_vol_name_placeholder": "Nome volume personalizzato",
        "save_btn": "Salva",
        "cluster_label": "Dim. Cluster (Byte):",
        "slack_label": "Byte di Slack:",
        "search_time_label": "Tempo Max Ricerca (sec):",
        "split_depth_label": "Profondità divisione dir:",
        "skip_empty_chk": "Salta cartelle / file vuoti",
        "skip_unreadable_chk": "Salta file illeggibili (Tollerante)",
        "integrate_ctx_chk": "Integra nel menu contestuale di Windows Explorer",
        "rule_wins_chk": "Il raggruppamento basato su regole vince:",
        "enable_par3_chk": "Abilita protezione di parità PAR3:",
        "par3_block_size": "Dimensione blocco PAR3 (Byte):",
        "par3_redundancy": "Percentuale ridondanza PAR3 (%):",
        "language_label": "Lingua:",
        "grouping_rules_frame": "Regole di raggruppamento file / cartelle",
        "pattern_col": "Pattern",
        "files_col": "File",
        "folders_col": "Cartelle",
        "type_col": "Tipo",
        "rule_pattern_placeholder": "Pattern della regola (*.mp3 o ^[0-9].*\\..*$)",
        "match_files_chk": "Corrispondenza File",
        "match_folders_chk": "Corrispondenza Cartelle",
        "regex_pattern_chk": "Pattern Regex",
        "add_rule_btn": "Aggiungi Regola",
        "remove_selected_btn": "Rimuovi Selezionati",
        "restart_notice": "Preferenze della lingua salvate. Riavvia Burn to the Brim per applicare le modifiche.",
        "restart_title": "Lingua Modificata",
        "dir_setup_group": "Configurazione Directory",
        "target_folder": "Cartella di destinazione:",
        "semantic_prompt": "Prompt semantico:",
        "move_files_chk": "Sposta cartelle/file adattati nella cartella di destinazione",
        "create_symlinks_chk": "Crea collegamenti simbolici nella cartella di destinazione (Predefinito)",
        "span_chk": "Suddividi su più volumi (Volume_1, Volume_2, ecc.)",
        "trace_chk": "Abilita log diagnostico dettagliato del risolutore (Trace)",
        "progress_frame_title": "Capacità del mezzo riempito",
        "filled_label": "Riempito: 0.00%",
        "log_frame_title": "Log di stato e del risolutore",
        "time_left_label": "Tempo Rimasto: --:--",
        "test_btn": "Test",
        "start_btn": "Avvia",
        "stop_btn": "Ferma",
        "pref_title": "Preferenze",
        "create_iso_btn": "Crea ISO...",
        "help_btn": "Aiuto",
        "about_btn": "Informazioni...",
        "import_json_btn": "Importa JSON...",
        "verify_restore_btn": "Verifica & Ripristina...",
        "tooltip_import_json": "Importa e analizza i file di indice JSON offline per visualizzare i dettagli dei file del volume risolto.",
        "tooltip_verify_restore": "Verifica i volumi risolti o copia e ripristina i file danneggiati utilizzando gli archivi PAR3.",
        "tooltip_test": "Simula il riempimento senza eseguire operazioni sui file sul disco per testare l'utilizzo del volume.",
        "tooltip_start": "Esegui il risolutore e organizza file/cartelle in base alle preferenze.",
        "tooltip_stop": "Interrompe il processo di compressione o di organizzazione dei file attualmente in esecuzione.",
        "tooltip_create_iso": "Crea immagini del file system ISO dai volumi risolti.",
        "tooltip_source": "Percorsi delle cartelle da comprimere e consolidare, separati da punti e virgola.",
        "tooltip_target": "Percorso di destinazione in cui verranno organizzati e scritti i volumi.",
        "tooltip_semantic": "Inserisci un prompt come 'keep audio together' per analizzare i file tramite MiniLM.",
        "tooltip_move": "Sposta/taglia i file/cartelle originali direttamente nelle cartelle del volume di destinazione.",
        "tooltip_symlink": "Crea collegamenti simbolici del file system nelle cartelle del volume di destinazione (non distruttivo).",
        "tooltip_span": "Abilita la suddivisione del contenuto in più cartelle denominate in modo sequenziale.",
        "tooltip_trace": "Mostra log di diagnostica dettagliati ed metriche di prestazioni.",
        "manage_src_folders_title": "Gestisci cartelle di origine",
        "help_guide_title": "Guida all'aiuto di Burn to the Brim (BTTB)",
        "help_title": "Aiuto - Burn to the Brim",
        "about_title": "Informazioni su Burn to the Brim",
        "media_size_label": "Dimensione Supporto:",
        "save_cv_tooltip": "Salva la capacità corrente, il settore e lo slack come volume personalizzato con nome",
        "skip_empty_label": "Salta File/Dir vuoti:",
        "dark_theme_chk": "Abilita Tema Scuro:",
        "add_src_dir_title": "Aggiungi directory di origine",
        "edit_src_dir_title": "Modifica directory di origine",
        "start_tooltip": "Inizia a organizzare i file in volumi ottimali",
        "test_tooltip": "Simula l'organizzazione dei file e calcola le metriche senza modificare i file sul disco",
        "stop_tooltip": "Ferma il risolutore corrente o l'operazione di copia",
        "rel_path_col": "Percorso relativo / Categoria",
        "size_col": "Dimensione",
        "fitted_status_col": "Stato adattamento",
        "source_tooltip": "Directory di origine contenente i file da organizzare",
        "browse_src_tooltip": "Sfoglia per la directory di origine",
        "target_tooltip": "Directory di destinazione in cui verranno creati i volumi organizzati",
        "browse_dest_tooltip": "Sfoglia per la directory di destinazione",
        "select_dest_dir_title": "Seleziona directory di destinazione",
        "semantic_placeholder": "es. mantieni simili contenuti insieme",
        "semantic_tooltip": "Specifica una descrizione semantica per raggruppare file simili utilizzando MiniLM AI",
        "move_tooltip": "Sposta i file nei volumi di destinazione invece di copiarli o creare collegamenti simbolici",
        "symlink_tooltip": "Crea collegamenti simbolici dei file nella destinazione invece di copiarli",
        "span_tooltip": "Adatta i file rimanenti in più volumi in sequenza invece del solo primo volume",
        "trace_tooltip": "Mostra diagnostica dettagliata e metriche di prestazioni nei log",
        "capacity_recommend_prompt_gtk": "L'elemento scansionato più grande richiede una capacità di volume di almeno %.2f MB.\n\nScegli un'azione:",
        "btn_resize": "Ridimensiona",
        "btn_skip": "Salta file",
        "btn_cancel": "Annulla",
        "log_warn_dir_unreadable_1": "Attenzione: Directory '",
        "log_warn_dir_unreadable_2": "' non è leggibile (",
        "log_scan_aborted_unreadable_dir": "Scansione interrotta dall'utente a causa di una directory non leggibile.",
        "log_warn_item_unreadable_1": "Attenzione: File o cartella in '",
        "log_warn_item_unreadable_2": "' non è leggibile (",
        "log_scan_aborted_unreadable_item": "Scansione interrotta dall'utente a causa di un elemento non leggibile.",
        "log_warn_failed_iterate_dir_1": "Attenzione: Impossibile iterare la directory '",
        "log_warn_failed_iterate_dir_2": "' (",
        "log_scan_aborted_failed_iterate": "Scansione interrotta dall'utente a causa del fallimento dell'iterazione della directory.",
        "log_scanning_folder": "Scansione della cartella: ",
        "log_found_items": "Elementi trovati da adattare: ",
        "log_search_time_limit_exceeded_1": "Limite di tempo di ricerca superato (",
        "log_search_time_limit_exceeded_2": " secondi).",
        "log_new_best_space_utilization": "Nuovo miglior utilizzo dello spazio: ",
        "log_selection_within_slack_1": "Trovata selezione entro la tolleranza di slack (",
        "log_selection_within_slack_2": "%). Termino la ricerca in anticipo.",
        "log_no_items_to_fit": "Nessun elemento da adattare.",
        "log_auto_volume_cap_set_1": "Capacità auto volume impostata a: ",
        "log_auto_volume_cap_set_2": " byte (",
        "log_warn_item_exceeds_cap_1": "Attenzione: L'elemento '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " byte) è più grande della dimensione del volume di destinazione (",
        "log_capacity_adapted_1": "Capacità adattata a ",
        "log_capacity_adapted_2": " byte. Riprovo il risolutore...",
        "log_capacity_skip_larger": "File più grandi saltati. Riprovo il risolutore...",
        "log_solver_aborted_files_exceed_cap": "Risolutore interrotto: il set di dati contiene file che superano la capacità.",
        "log_solver_aborted_files_exceed_cap_adapt": "Risolutore interrotto: il set di dati contiene file che superano la capacità. Adatta la capacità o abilita il riprova interattivo.",
        "log_volume_packing_estimation_header": "--- Stima della compressione del volume ---",
        "log_total_dataset_size_1": "Dimensione totale del set di dati: ",
        "log_total_dataset_size_2": " byte (",
        "log_target_volume_cap_1": "Capacità del volume di destinazione: ",
        "log_target_volume_cap_2": " byte (",
        "log_theoretical_min_vols": "Volumi minimi teorici richiesti: ",
        "log_medium_capacity_1": "Capacità del supporto: ",
        "log_medium_capacity_2": " settori (",
        "log_solving_for_vol_header_1": "\r\n--- RISOLUZIONE DEL VOLUME ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "Risoluzione della selezione ottimale dei bin...",
        "log_finished_solving_1": "Risoluzione completata. Tempo trascorso: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "Soluzione a volume singolo: forzatura della selezione di tutti i file per garantire l'inclusione al 100%...",
        "log_backtracking_failed_fallback_greedy": "Il risolutore con backtracking non ha trovato un adattamento (o è andato in timeout). Ricaduta sull'allocazione greedy per garantire che tutti i file rimanenti siano inclusi...",
        "log_forcing_progress_exceeds_cap_1": "Forzatura del progresso: compressione dell'elemento rimanente più grande della capacità del supporto (",
        "log_optimal_selection_covers": "La selezione ottimale copre: ",
        "log_selected_items_for_volume_1": "Elementi selezionati per il volume ",
        "log_selected_items": "Elementi selezionati:",
        "log_bytes_suffix": " byte)",
        "log_organizing_item": "Organizzazione dell'elemento: ",
        "log_failed_symlink_retry_copy_1": "Impossibile creare il collegamento simbolico per ",
        "log_failed_symlink_retry_copy_2": " (riprovo con la copia)",
        "log_failed_organize_item_1": "Impossibile organizzare ",
        "log_generating_par3_for_volume_1": "Generazione dei file di parità PAR3 per il Volume ",
        "log_success_par3_for_volume_1": "Creata con successo la protezione di parità PAR3 per il Volume ",
        "log_failed_par3_creation": "Creazione PAR3 fallita: ",
        "log_test_simulation_complete": "--- SIMULAZIONE DI TEST COMPLETATA ---",
        "log_no_files_written_on_disk": "Nessun file è stato copiato, collegato simbolicamente o spostato sul disco.",
        "log_completed_file_organization": "Organizzazione dei file completata.",
        "log_created_offline_json_index": "Creato file di indice JSON offline: ",
        "log_failed_create_json_index": "Impossibile creare il file di indice JSON: ",
        "log_generating_semantic_embeddings": "Generazione degli embedding semantici...",
        "log_failed_write_temp_embed_input": "Impossibile scrivere il file temporaneo di input degli embedding: ",
        "log_warn_python_embed_engine_failed": "Attenzione: Il motore di embedding python del sottoprocesso è fallito o python3 non è installato.",
        "log_semantic_grouping_fallback": "-> Il raggruppamento semantico ricadrà sulle metriche di corrispondenza delle stringhe locali.",
        "log_failed_read_temp_embed_output": "Impossibile leggere il file temporaneo di output degli embedding: ",
        "log_warn_embedded_output_empty": "Attenzione: L'output incorporato è vuoto.",
        "log_running_semantic_clustering": "Esecuzione del clustering semantico agglomerativo (soglia=0.6)...",
        "log_semantic_clustering_completed": "Clustering semantico completato. Gruppi consolidati totali: ",
        "about_comments": (
            "Burn to the Brim (BTTB) è un port moderno in C++20 della classica applicazione Delphi progettata per adattare in modo ottimale file e cartelle su supporti di memorizzazione di destinazione (CD, DVD, Blu-ray o USB).\n\n"
            "Caratteristiche in v4.6.0:\n"
            "- Icona dell'applicazione ad alta risoluzione completamente nuova (bttb.ico) e logo del sito web unificato (bttb.png)\n"
            "- Stack frame dello stato di ricerca minimizzati & limite di stack Win32 di 16MB (risolvendo gli overflow 0xC00000FD)\n"
            "- Limiti del buffer di logging espansi a 10MB per evitare la troncamento del log di traccia\n"
            "- Rendering GUI incrementale non bloccante & salto avvisi capacità file\n"
            "- Creazione di indici JSON offline e parser interattivo\n"
            "- Generazione e verifica opzionale di file di parità PAR3\n"
            "- Ripristino e riparazione basati sulla copia PAR3 bit-perfect\n"
            "- Supporto dei temi inclusa l'opzione del tema scuro standard su Linux GTK4\n"
            "- Calcolo del tempo rimanente stimato migliorato immediatamente all'avvio\n"
            "- Profili di volume personalizzati denominati e dimensionamento automatico dinamico del volume\n"
            "- Memoria delle impostazioni che ripristina l'ultima configurazione di volume selezionata\n"
            "- Sostituzioni dei conflitti di regole che consentono la vittoria del raggruppamento basato su regole o semantico\n"
            "- Tempo rimasto stimato in base alla velocità di trasferimento & indicatore spinner di attività dello stato\n"
            "- Compressione semantica consapevole dell'entropia basata sugli embedding MiniLM\n"
            "- Integrazione con il menu contestuale di Windows Explorer & supporto dei percorsi lunghi\n\n"
            "BTTB è completamente localizzato e traduce dinamicamente l'intera interfaccia utente basandosi su modelli gettext .po standard in tedesco, olandese, francese e spagnolo.\n\n"
            "Librerie ed Attribuzioni Utilizzate:\n"
            "- libpar3 (di Yutaka Sawada, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3 (del team BLAKE3, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS (di Christopher A. Taylor, BSD 3-Clause): https://github.com/catid/leopard\n"
            "- Libreria Galois Field (di James S. Plank): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "Autori: Sander Raaijmakers, Elwin Oost e il team di Burn to the Brim"
        ),
        "help_guide_text": (
            "1. Profondità di divisione della directory\n"
            "Determina il livello di nidificazione delle cartelle a cui gli elementi vengono divisi:\n"
            " - Profondità 0 (Predefinito): i file e le cartelle di livello superiore vengono trattati come elementi separati.\n"
            " - Profondità 1: la divisione avviene un livello più profondo, mantenendo intatte le cartelle di livello superiore ma dividendo le loro sottocartelle immediate.\n\n"
            "2. Tempo Massimo di Ricerca\n"
            "Il numero massimo di secondi in cui il risolutore con backtracking può essere eseguito. Se raggiunto, viene utilizzata la migliore selezione trovata fino a quel momento.\n\n"
            "3. Spanning Slack\n"
            "Consente la terminazione anticipata del risolutore quando un volume viene riempito entro questo numero di byte dalla capacità massima assoluta (ad es. 2048 byte).\n\n"
            "4. Regole di raggruppamento file/cartelle\n"
            "Forza gli elementi corrispondenti a rimanere raggruppati insieme sullo stesso volume (ad es. corrispondenti a '*.mp3' o regex '^album_.*').\n\n"
            "5. Cartelle di origine multiple (+)\n"
            "Fai clic su '+' per specificare più cartelle di origine. BTTB si comporta come se si trovassero in una singola cartella principale. I percorsi di origine nidificati vengono ignorati.\n\n"
            "6. Crea collegamenti simbolici\n"
            "Invece di copiare/spostare i file nella cartella di destinazione, BTTB crea collegamenti simbolici leggeri che rimandano ai file originali.\n\n"
            "7. Imballaggio Semantico Neurale & Guida alla Configurazione MiniLM\n"
            "Specificando un prompt semantico, BTTB raggruppa i file con contenuto simile utilizzando embedding di deep learning sensibili al contesto.\n"
            "Per utilizzare il modello neurale MiniLM preferito ad alta precisione, è necessario installare Python 3 e sentence-transformers:\n"
            " - Passaggio 1: assicurarsi che Python 3 e pip siano installati.\n"
            "   (Linux: eseguire 'sudo apt install python3 python3-pip python3-venv')\n"
            "   (Windows: installare da https://www.python.org/ e selezionare 'Aggiungi Python a PATH')\n"
            " - Passaggio 2: installare sentence-transformers tramite terminale o prompt dei comandi:\n"
            "   Opzione A (Consigliata per semplicità):\n"
            "     pip install sentence-transformers\n"
            "   Opzione B (Isolamento dell'ambiente virtuale):\n"
            "     python3 -m venv bttb_env\n"
            "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
            "     pip install sentence-transformers\n"
            " - Passaggio 3: riavviare Burn to the Brim per caricare automaticamente MiniLM! Se non trovato, BTTB ripiega elegantemente su un proiettore TF-IDF a caratteri localizzato."
        )
    }

    # Now let's define Greek (el)
    translations["el"] = {
        "log_start_restore": "Έναρξη επαναφοράς/επισκευής τόμου...\n",
        "log_volume_path": "Διαδρομή τόμου: ",
        "log_recovery_path": "Διαδρομή ανάκτησης: ",
        "log_par3_base": "Βασικό όνομα PAR3: ",
        "log_copying_repair": "Αντιγραφή περιεχομένων τόμου και εκτέλεση επισκευής. Παρακαλώ περιμένετε...\n",
        "log_errors": "Καταγραφές/Σφάλματα: ",
        "log_success_restore": "\nΕΠΙΤΥΧΙΑ: Ο τόμος αντιγράφηκε και επισκευάστηκε με επιτυχία σε ξεχωριστό φάκελο ανάκτησης!\n",
        "log_fail_restore": "\nΑΠΟΤΥΧΙΑ: Η επαναφορά ή η επισκευή απέτυχε.\n",
        "log_start_verify": "Έναρξη επαλήθευσης τόμου...\n",
        "log_success_verify": "\nΕΠΙΤΥΧΙΑ: Όλα τα αρχεία επαληθεύτηκαν και είναι καθαρά/bit-perfect!\n",
        "log_fail_verify_status": "\nΗ επαλήθευση εντόπισε προβλήματα (κατάσταση ",
        "log_damaged_files": "Βρέθηκαν κατεστραμμένα αρχεία ή αρχεία που λείπουν:\n",
        "log_index_fail": "Δεν εντοπίστηκαν μεμονωμένα κατεστραμμένα αρχεία, αλλά η επαλήθευση ευρετηρίου απέτυχε. Το ίδιο το αρχείο PAR3 ενδέχεται να έχει υποστεί ζημιά.\n",
        "verify_title": "Επαλήθευση & Επαναφορά Τόμων...",
        "label_vol_directory": "Κατάλογος Τόμου:",
        "browse_btn": "Αναζήτηση...",
        "select_vol_dir_title": "Επιλογή Καταλόγου Τόμου",
        "label_rec_directory": "Κατάλογος Ανάκτησης:",
        "select_rec_dir_title": "Επιλογή Καταλόγου Ανάκτησης",
        "label_par3_base": "Βασικό Όνομα PAR3:",
        "log_frame_title_verify": "Καταγραφή Επαλήθευσης & Επαναφοράς",
        "close_btn": "Κλείσιμο",
        "verify_only_btn": "Μόνο Επαλήθευση",
        "restore_repair_btn": "Επαναφορά & Επισκευή",
        "iso_title": "Δημιουργία Εικόνας ISO",
        "source_folder": "Φάκελος Προέλευσης:",
        "target_iso_file": "Αρχείο ISO Προορισμού:",
        "volume_label": "Ετικέτα Τόμου:",
        "execution_log": "Καταγραφή Εκτέλεσης",
        "generate_btn": "Δημιουργία",
        "complete_list_src_folders": "Πλήρης λίστα φακέλων προέλευσης:",
        "add_btn": "Προσθήκη...",
        "edit_btn": "Επεξεργασία...",
        "remove_btn": "Κατάργηση",
        "cancel_btn": "Ακύρωση",
        "ok_btn": "OK",
        "app_title": "Burn to the Brim",
        "app_version": "Έκδοση 4.6.0",
        "app_copyright": "Πνευματικά δικαιώματα © 2001-2026 Sander Raaijmakers, Elwin Oost και η ομάδα του Burn to the Brim",
        "select_medium_label": "Επιλογή Μέσου:",
        "auto_size": "Αυτόματο Μέγεθος",
        "custom_size": "Προσαρμοσμένο Μέγεθος",
        "capacity_label": "Χωρητικότητα:",
        "save_cv_label": "Αποθήκευση Προσαρμ. Τόμου:",
        "custom_vol_name_placeholder": "Όνομα προσαρμοσμένου τόμου",
        "save_btn": "Αποθήκευση",
        "cluster_label": "Μέγεθος Cluster (Bytes):",
        "slack_label": "Bytes Slack (Toler):",
        "search_time_label": "Μέγιστος Χρόνος Αναζήτησης (sec):",
        "split_depth_label": "Βάθος Διαχωρισμού Καταλόγου:",
        "skip_empty_chk": "Παράλειψη κενών φακέλων / αρχείων",
        "skip_unreadable_chk": "Παράλειψη μη αναγνώσιμων αρχείων (Επιεικής)",
        "integrate_ctx_chk": "Ενσωμάτωση στο μενού επιλογών των Windows Explorer",
        "rule_wins_chk": "Η ομαδοποίηση βάσει κανόνων υπερισχύει:",
        "enable_par3_chk": "Ενεργοποίηση προστασίας ισοτιμίας PAR3:",
        "par3_block_size": "Μέγεθος μπλοκ PAR3 (Bytes):",
        "par3_redundancy": "Ποσοστό πλεονασμού PAR3 (%):",
        "language_label": "Γλώσσα:",
        "grouping_rules_frame": "Κανόνες ομαδοποίησης αρχείων / φακέλων",
        "pattern_col": "Μοτίβο",
        "files_col": "Αρχεία",
        "folders_col": "Φάκελοι",
        "type_col": "Τύπος",
        "rule_pattern_placeholder": "Μοτίβο κανόνα (*.mp3 ή ^[0-9].*\\..*$)",
        "match_files_chk": "Ταίριασμα Αρχείων",
        "match_folders_chk": "Ταίριασμα Φακέλων",
        "regex_pattern_chk": "Μοτίβο Regex",
        "add_rule_btn": "Προσθήκη Κανόνα",
        "remove_selected_btn": "Κατάργηση Επιλεγμένων",
        "restart_notice": "Οι προτιμήσεις γλώσσας αποθηκεύτηκαν. Παρακαλώ επανεκκινήστε το Burn to the Brim για να εφαρμόσετε τις αλλαγές.",
        "restart_title": "Η γλώσσα άλλαξε",
        "dir_setup_group": "Ρύθμιση Καταλόγων",
        "target_folder": "Φάκελος Προορισμού:",
        "semantic_prompt": "Σημασιολογικό prompt:",
        "move_files_chk": "Μετακίνηση προσαρμοσμένων φακέλων/αρχείων στον φάκελο προορισμού",
        "create_symlinks_chk": "Δημιουργία συμβολικών συνδέσμων στον φάκελο προορισμού (Προεπιλογή)",
        "span_chk": "Κατανομή σε πολλαπλούς τόμους (Volume_1, Volume_2, κ.λπ.)",
        "trace_chk": "Ενεργοποίηση λεπτομερούς διαγνωστικής καταγραφής (Trace)",
        "progress_frame_title": "Χωρητικότητα προσαρμοσμένου μέσου",
        "filled_label": "Γέμισε: 0.00%",
        "log_frame_title": "Καταγραφή κατάστασης και επίλυσης",
        "time_left_label": "Χρόνος που απομένει: --:--",
        "test_btn": "Δοκιμή",
        "start_btn": "Έναρξη",
        "stop_btn": "Διακοπή",
        "pref_title": "Προτιμήσεις",
        "create_iso_btn": "Δημιουργία ISO...",
        "help_btn": "Βοήθεια",
        "about_btn": "Σχετικά...",
        "import_json_btn": "Εισαγωγή JSON...",
        "verify_restore_btn": "Επαλήθευση & Επαναφορά...",
        "tooltip_import_json": "Εισαγωγή και ανάλυση αρχείων ευρετηρίου JSON εκτός σύνδεσης για προβολή λεπτομερειών των αρχείων του επιλυμένου τόμου.",
        "tooltip_verify_restore": "Επαληθεύστε τους επιλυμένους τόμους ή αντιγράψτε και επαναφέρετε κατεστραμμένα αρχεία χρησιμοποιώντας αρχειοθήκες PAR3.",
        "tooltip_test": "Προσομοίωση συσκευασίας χωρίς εκτέλεση λειτουργιών αρχείων στο δίσκο για δοκιμή της χρήσης του τόμου.",
        "tooltip_start": "Εκτελέστε τον επιλυτή και οργανώστε αρχεία/φακέλους σύμφωνα με τις προτιμήσεις.",
        "tooltip_stop": "Διακοπή της τρέχουσας διαδικασίας συσκευασίας ή οργάνωσης αρχείων.",
        "tooltip_create_iso": "Δημιουργήστε εικόνες συστήματος αρχείων ISO από τους επιλυμένους τόμους.",
        "tooltip_source": "Διαδρομές φακέλων προς συσκευασία και ενοποίηση, διαχωρισμένες με ελληνικό ερωτηματικό.",
        "tooltip_target": "Διαδρομή προορισμού όπου θα οργανωθούν και θα γραφτούν οι τόμοι.",
        "tooltip_semantic": "Εισαγάγετε ένα prompt όπως 'keep audio together' για να αναλύσετε αρχεία μέσω MiniLM.",
        "tooltip_move": "Μετακινήστε/αποκόψτε τα αρχικά αρχεία/φακέλους απευθείας στους φακέλους του τόμου προορισμού.",
        "tooltip_symlink": "Δημιουργήστε συμβολικούς συνδέσμους συστήματος αρχείων στους φακέλους του τόμου προορισμού (μη καταστροφικό).",
        "tooltip_span": "Ενεργοποιήστε την κατανομή περιεχομένου σε πολλούς φακέλους με διαδοχική ονομασία.",
        "tooltip_trace": "Εμφάνιση εξαιρετικά λεπτομερών διαγνωστικών στοιχείων και μετρήσεων απόδοσης.",
        "manage_src_folders_title": "Διαχείριση φακέλων προέλευσης",
        "help_guide_title": "Οδηγός βοήθειας Burn to the Brim (BTTB)",
        "help_title": "Βοήθεια - Burn to the Brim",
        "about_title": "Σχετικά με το Burn to the Brim",
        "media_size_label": "Μέγεθος Μέσου:",
        "save_cv_tooltip": "Αποθήκευση τρέχουσας χωρητικότητας, τομέα και slack ως επώνυμο προσαρμοσμένο τόμο",
        "skip_empty_label": "Παράλειψη κενών αρχείων/καταλόγων:",
        "dark_theme_chk": "Ενεργοποίηση Σκούρου Θέματος:",
        "add_src_dir_title": "Προσθήκη καταλόγου προέλευσης",
        "edit_src_dir_title": "Επεξεργασία καταλόγου προέλευσης",
        "start_tooltip": "Ξεκινήστε την οργάνωση αρχείων σε βέλτιστους τόμους",
        "test_tooltip": "Προσομοιώστε την οργάνωση αρχείων και υπολογίστε μετρήσεις χωρίς τροποποίηση αρχείων στο δίσκο",
        "stop_tooltip": "Διακόψτε τον τρέχοντα επιλυτή ή τη λειτουργία αντιγραφής",
        "rel_path_col": "Σχετική διαδρομή / Κατηγορία",
        "size_col": "Μέγεθος",
        "fitted_status_col": "Κατάσταση προσαρμογής",
        "source_tooltip": "Κατάλογος προέλευσης που περιέχει τα αρχεία προς οργάνωση",
        "browse_src_tooltip": "Αναζήτηση καταλόγου προέλευσης",
        "target_tooltip": "Κατάλογος προορισμού όπου θα δημιουργηθούν οι οργανωμένοι τόμοι",
        "browse_dest_tooltip": "Αναζήτηση καταλόγου προορισμού",
        "select_dest_dir_title": "Επιλογή καταλόγου προορισμού",
        "semantic_placeholder": "π.χ. διατήρηση παρόμοιου περιεχομένου μαζί",
        "semantic_tooltip": "Καθορίστε μια σημασιολογική περιγραφή για την ομαδοποίηση παρόμοιων αρχείων χρησιμοποιώντας το MiniLM AI",
        "move_tooltip": "Μετακινήστε αρχεία στους τόμους προορισμού τους αντί να τα αντιγράψετε ή να δημιουργήσετε συμβολικούς συνδέσμους",
        "symlink_tooltip": "Δημιουργήστε συμβολικούς συνδέσμους των αρχείων στον προορισμό τους αντί να τα αντιγράψετε",
        "span_tooltip": "Προσαρμόστε τα υπόλοιπα αρχεία σε πολλούς τόμους διαδοχικά αντί μόνο στον πρώτο τόμο",
        "trace_tooltip": "Εμφανίστε λεπτομερή διαγνωστικά στοιχεία και μετρήσεις απόδοσης στις καταγραφές",
        "capacity_recommend_prompt_gtk": "Το μεγαλύτερο σαρωμένο στοιχείο απαιτεί χωρητικότητα τόμου τουλάχιστον %.2f MB.\n\nΕπιλέξτε μια ενέργεια:",
        "btn_resize": "Αλλαγή μεγέθους",
        "btn_skip": "Παράλειψη αρχείων",
        "btn_cancel": "Ακύρωση",
        "log_warn_dir_unreadable_1": "Προειδοποίηση: Ο κατάλογος '",
        "log_warn_dir_unreadable_2": "' δεν είναι αναγνώσιμος (",
        "log_scan_aborted_unreadable_dir": "Η σάρωση διακόπηκε από το χρήστη λόγω μη αναγνώσιμου καταλόγου.",
        "log_warn_item_unreadable_1": "Προειδοποίηση: Το αρχείο ή ο φάκελος στο '",
        "log_warn_item_unreadable_2": "' δεν είναι αναγνώσιμος (",
        "log_scan_aborted_unreadable_item": "Η σάρωση διακόπηκε από το χρήστη λόγω μη αναγνώσιμου στοιχείου.",
        "log_warn_failed_iterate_dir_1": "Προειδοποίηση: Αποτυχία επανάληψης καταλόγου '",
        "log_warn_failed_iterate_dir_2": "' (",
        "log_scan_aborted_failed_iterate": "Η σάρωση διακόπηκε από το χρήστη λόγω αποτυχίας επανάληψης καταλόγου.",
        "log_scanning_folder": "Σάρωση φακέλου: ",
        "log_found_items": "Βρέθηκαν στοιχεία προς προσαρμογή: ",
        "log_search_time_limit_exceeded_1": "Υπέρβαση χρονικού ορίου αναζήτησης (",
        "log_search_time_limit_exceeded_2": " δευτερόλεπτα).",
        "log_new_best_space_utilization": "Νέα βέλτιστη χρήση χώρου: ",
        "log_selection_within_slack_1": "Βρέθηκε επιλογή εντός της ανοχής slack (",
        "log_selection_within_slack_2": "%). Πρόωρος τερματισμός αναζήτησης.",
        "log_no_items_to_fit": "Δεν υπάρχουν στοιχεία προς προσαρμογή.",
        "log_auto_volume_cap_set_1": "Η χωρητικότητα αυτόματου τόμου ορίστηκε σε: ",
        "log_auto_volume_cap_set_2": " bytes (",
        "log_warn_item_exceeds_cap_1": "Προειδοποίηση: Το στοιχείο '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " bytes) είναι μεγαλύτερο από το μέγεθος του τόμου προορισμού (",
        "log_capacity_adapted_1": "Η χωρητικότητα προσαρμόστηκε σε ",
        "log_capacity_adapted_2": " bytes. Επανάληψη επίλυσης...",
        "log_capacity_skip_larger": "Παραλείφθηκαν τα μεγαλύτερα αρχεία. Επανάληψη επίλυσης...",
        "log_solver_aborted_files_exceed_cap": "Ο επιλύτης διακόπηκε: το σύνολο δεδομένων περιέχει αρχεία που υπερβαίνουν τη χωρητικότητα.",
        "log_solver_aborted_files_exceed_cap_adapt": "Ο επιλύτης διακόπηκε: το σύνολο δεδομένων περιέχει αρχεία που υπερβαίνουν τη χωρητικότητα. Προσαρμόστε τη χωρητικότητα ή ενεργοποιήστε την διαδραστική επανάληψη.",
        "log_volume_packing_estimation_header": "--- Εκτίμηση Συσκευασίας Τόμου ---",
        "log_total_dataset_size_1": "Συνολικό μέγεθος συνόλου δεδομένων: ",
        "log_total_dataset_size_2": " bytes (",
        "log_target_volume_cap_1": "Χωρητικότητα τόμου προορισμού: ",
        "log_target_volume_cap_2": " bytes (",
        "log_theoretical_min_vols": "Θεωρητικός ελάχιστος αριθμός τόμων που απαιτούνται: ",
        "log_medium_capacity_1": "Χωρητικότητα μέσου: ",
        "log_medium_capacity_2": " τομείς (",
        "log_solving_for_vol_header_1": "\r\n--- ΕΠΙΛΥΣΗ ΓΙΑ ΤΟΝ ΤΟΜΟ ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "Επίλυση βέλτιστης επιλογής bin...",
        "log_finished_solving_1": "Η επίλυση ολοκληρώθηκε. Χρόνος που παρήλθε: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "Λύση μονού τόμου: αναγκαστική επιλογή όλων των αρχείων για εγγύηση 100% συμπερίληψης...",
        "log_backtracking_failed_fallback_greedy": "Ο επιλύτης backtracking δεν βρήκε προσαρμογή (ή έληξε ο χρόνος). Επιστροφή σε άπληστη κατανομή για να διασφαλιστεί ότι περιλαμβάνονται όλα τα υπόλοιπα αρχεία...",
        "log_forcing_progress_exceeds_cap_1": "Αναγκαστική πρόοδος: συσκευασία εναπομένοντος στοιχείου μεγαλύτερου από τη χωρητικότητα του μέσου (",
        "log_optimal_selection_covers": "Η βέλτιστη επιλογή καλύπτει: ",
        "log_selected_items_for_volume_1": "Επιλεγμένα στοιχεία για τον Τόμο ",
        "log_selected_items": "Επιλεγμένα στοιχεία:",
        "log_bytes_suffix": " bytes)",
        "log_organizing_item": "Οργάνωση στοιχείου: ",
        "log_failed_symlink_retry_copy_1": "Αποτυχία δημιουργίας συμβολικού συνδέσμου για ",
        "log_failed_symlink_retry_copy_2": " (επανάληψη με αντιγραφή)",
        "log_failed_organize_item_1": "Αποτυχία οργάνωσης ",
        "log_generating_par3_for_volume_1": "Δημιουργία αρχείων ισοτιμίας PAR3 για τον Τόμο ",
        "log_success_par3_for_volume_1": "Δημιουργήθηκε με επιτυχία προστασία ισοτιμίας PAR3 για τον Τόμο ",
        "log_failed_par3_creation": "Η δημιουργία PAR3 απέτυχε: ",
        "log_test_simulation_complete": "--- Η ΔΟΚΙΜΑΣΤΙΚΗ ΠΡΟΣΟΜΟΙΩΣΗ ΟΛΟΚΛΗΡΩΘΗΚΕ ---",
        "log_no_files_written_on_disk": "Δεν αντιγράφηκαν, μετακινήθηκαν ή συνδέθηκαν συμβολικά αρχεία στο δίσκο.",
        "log_completed_file_organization": "Ολοκληρώθηκε η οργάνωση αρχείων.",
        "log_created_offline_json_index": "Δημιουργήθηκε αρχείο ευρετηρίου JSON εκτός σύνδεσης: ",
        "log_failed_create_json_index": "Αποτυχία δημιουργίας αρχείου ευρετηρίου JSON: ",
        "log_generating_semantic_embeddings": "Δημιουργία σημασιολογικών ενσωματώσεων...",
        "log_failed_write_temp_embed_input": "Αποτυχία εγγραφής προσωρινού αρχείου εισαγωγής ενσωμάτωσης: ",
        "log_warn_python_embed_engine_failed": "Προειδοποίηση: Η μηχανή ενσωμάτωσης python απέτυχε ή η python3 δεν είναι εγκατεστημένη.",
        "log_semantic_grouping_fallback": "-> Η σημασιολογική ομαδοποίηση θα επιστρέψει σε τοπικές μετρήσεις αντιστοίχισης συμβολοσειρών.",
        "log_failed_read_temp_embed_output": "Αποτυχία ανάγνωσης προσωρινού αρχείου εξόδου ενσωμάτωσης: ",
        "log_warn_embedded_output_empty": "Προειδοποίηση: Η ενσωματωμένη έξοδος είναι κενή.",
        "log_running_semantic_clustering": "Εκτέλεση συσσωρευτικής σημασιολογικής ομαδοποίησης (κατώφλι=0.6)...",
        "log_semantic_clustering_completed": "Η σημασιολογική ομαδοποίηση ολοκληρώθηκε. Συνολικές ενοποιημένες ομάδες: ",
        "about_comments": (
            "Το Burn to the Brim (BTTB) είναι μια σύγχρονη μεταφορά σε C++20 της κλασικής εφαρμογής Delphi, σχεδιασμένη για τη βέλτιστη τοποθέτηση αρχείων και φακέλων σε μέσα αποθήκευσης προορισμού (CD, DVD, Blu-ray ή USB).\n\n"
            "Χαρακτηριστικά στην έκδοση v4.6.0:\n"
            "- Ολοκαίνουργιο εικονίδιο εφαρμογής υψηλής ανάλυσης (bttb.ico) και ενοποιημένο λογότυπο ιστότοπου (bttb.png)\n"
            "- Ελαχιστοποιημένα stack frames κατάστασης αναζήτησης & όριο stack 16MB Win32 (διόρθωση σφαλμάτων υπερχείλισης 0xC00000FD)\n"
            "- Διευρυμένα όρια buffer καταγραφής στα 10MB για την αποφυγή περικοπής των αρχείων καταγραφής\n"
            "- Μη μπλοκαρισμένη σταδιακή απόδοση GUI & παράκαμψη προειδοποιήσεων χωρητικότητας αρχείων\n"
            "- Δημιουργία ευρετηρίου JSON εκτός σύνδεσης και διαδραστικός αναλυτής\n"
            "- Προαιρετική δημιουργία και επαλήθευση αρχείων ισοτιμίας PAR3\n"
            "- Bit-perfect αποκατάσταση και επισκευή βάσει αντιγραφής PAR3\n"
            "- Υποστήριξη θεμάτων, συμπεριλαμβανομένων των τυπικών επιλογών σκούρου θέματος στο Linux GTK4\n"
            "- Βελτιωμένος υπολογισμός εκτιμώμενου υπολειπόμενου χρόνου αμέσως κατά την εκκίνηση\n"
            "- Επώνυμα προφίλ προσαρμοσμένων τόμων & δυναμικό αυτόματο μέγεθος τόμου\n"
            "- Μνήμη ρυθμίσεων που επαναφέρει την τελευταία επιλεγμένη διαμόρφωση τόμου\n"
            "- Παρακάμψεις συγκρούσεων κανόνων που επιτρέπουν την επικράτηση της ομαδοποίησης βάσει κανόνων ή της σημασιολογικής ομαδοποίησης\n"
            "- Εκτιμώμενος υπολειπόμενος χρόνος βάσει ρυθμού μεταφοράς & δείκτης δραστηριότητας κατάστασης\n"
            "- Σημασιολογική συσκευασία με επίγνωση εντροπίας βάσει ενσωματώσεων MiniLM\n"
            "- Ενσωμάτωση στο μενού επιλογών των Windows Explorer & υποστήριξη μεγάλων διαδρομών\n\n"
            "Το BTTB είναι πλήρως μεταφρασμένο και προσαρμόζει δυναμικά ολόκληρη τη διεπαφή χρήστη με βάση τα τυπικά πρότυπα gettext .po σε Γερμανικά, Ολλανδικά, Γαλλικά και Ισπανικά.\n\n"
            "Βιβλιοθήκες και Συνεισφορές που Χρησιμοποιήθηκαν:\n"
            "- libpar3 (από τον Yutaka Sawada, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3 (από την ομάδα BLAKE3, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS (από τον Christopher A. Taylor, BSD 3-Clause): https://github.com/catid/leopard\n"
            "- Βιβλιοθήκη Galois Field (από τον James S. Plank): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "Συγγραφείς: Sander Raaijmakers, Elwin Oost και η ομάδα του Burn to the Brim"
        ),
        "help_guide_text": (
            "1. Βάθος Διαχωρισμού Καταλόγου\n"
            "Καθορίζει το επίπεδο φωλιάσματος φακέλων στο οποίο διαχωρίζονται τα στοιχεία:\n"
            " - Βάθος 0 (Προεπιλογή): Τα αρχεία και οι φάκελοι κορυφαίου επιπέδου αντιμετωπίζονται ως ξεχωριστά στοιχεία.\n"
            " - Βάθος 1: Ο διαχωρισμός πραγματοποιείται ένα επίπεδο βαθύτερα, διατηρώντας ανέπαφους τους φακέλους κορυφαίου επιπέδου αλλά διαχωρίζοντας τους άμεσους υποφακέλους τους.\n\n"
            "2. Μέγιστος Χρόνος Αναζήτησης\n"
            "Τα μέγιστα δευτερόλεπτα που επιτρέπεται να εκτελείται ο επιλύτης backtracking. Εάν επιτευχθεί το όριο, χρησιμοποιείται η καλύτερη επιλογή που βρέθηκε μέχρι εκείνο το σημείο.\n\n"
            "3. Spanning Slack\n"
            "Επιτρέπει τον πρόωρο τερματισμό του επιλύτη μόλις ένας τόμος γεμίσει εντός αυτού του αριθμού bytes από την απόλυτη μέγιστη χωρητικότητα (π.χ. 2048 bytes).\n\n"
            "4. Κανόνες Ομαδοποίησης Αρχείων/Φακέλων\n"
            "Αναγκάζει τα στοιχεία που ταιριάζουν να παραμένουν ομαδοποιημένα στον ίδιο τόμο (π.χ. ταιριάζοντας με '*.mp3' ή regex '^album_.*').\n\n"
            "5. Πολλαπλοί Φάκελοι Προέλευσης (+)\n"
            "Κάντε κλικ στο '+' για να καθορίσετε πολλαπλούς φακέλους προέλευσης. Το BTTB συμπεριφέρεται σαν να βρίσκονται σε έναν ενιαίο ριζικό φάκελο. Οι φωλιασμένες διαδρομές προέλευσης αγνοούνται.\n\n"
            "6. Δημιουργία Συμβολικών Συνδέσμων\n"
            "Αντί να αντιγράφει/μετακινεί αρχεία στον φάκελο προορισμού, το BTTB δημιουργεί ελαφρούς συμβολικούς συνδέσμους που δείχνουν πίσω στα αρχικά σας αρχεία.\n\n"
            "7. Νευρωνική Σημασιολογική Συσκευασία & Οδηγός Ρύθμισης MiniLM\n"
            "Καθορίζοντας ένα σημασιολογικό prompt, το BTTB ομαδοποιεί αρχεία με παρόμοιο περιεχόμενο χρησιμοποιώντας σημασιολογικές ενσωματώσεις βαθιάς μάθησης με επίγνωση του πλαισίου.\n"
            "Για να χρησιμοποιήσετε το προτιμώμενο νευρωνικό μοντέλο MiniLM υψηλής ακρίβειας, πρέπει να εγκαταστήσετε την Python 3 και το sentence-transformers:\n"
            " - Βήμα 1: Βεβαιωθείτε ότι είναι εγκατεστημένα τα Python 3 & pip.\n"
            "   (Linux: εκτελέστε 'sudo apt install python3 python3-pip python3-venv')\n"
            "   (Windows: Εγκαταστήστε από το https://www.python.org/ και επιλέξτε 'Add Python to PATH')\n"
            " - Βήμα 2: Εγκαταστήστε το sentence-transformers μέσω τερματικού ή γραμμής εντολών:\n"
            "   Επιλογή Α (Συνιστάται για απλότητα):\n"
            "     pip install sentence-transformers\n"
            "   Επιλογή Β (Απομόνωση εικονικού περιβάλλοντος):\n"
            "     python3 -m venv bttb_env\n"
            "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
            "     pip install sentence-transformers\n"
            " - Βήμα 3: Επανεκκινήστε το Burn to the Brim για αυτόματη φόρτωση του MiniLM! Εάν δεν βρεθεί, το BTTB επιστρέφει ομαλά σε έναν τοπικό προβολέα χαρακτήρων TF-IDF."
        )
    }

    # Now let's define Latin (la)
    translations["la"] = {
        "log_start_restore": "Instauratio/reparatio voluminis incipit...\n",
        "log_volume_path": "Volume iter: ",
        "log_recovery_path": "Receptio iter: ",
        "log_par3_base": "PAR3 nomen: ",
        "log_copying_repair": "Contenta voluminis exscribuntur et reparatio initur. Quaeso expecta...\n",
        "log_errors": "Acta/Errores: ",
        "log_success_restore": "\nSUCCESSUS: Volumen prospere exscriptum et reparatum est in alio folder receptionis!\n",
        "log_fail_restore": "\nFAILURA: Instauratio vel reparatio defecit.\n",
        "log_start_verify": "Verificatio voluminis incipit...\n",
        "log_success_verify": "\nSUCCESSUS: Omnia lima verificata sunt et munda/bit-perfecta!\n",
        "log_fail_verify_status": "\nVerificatio errata detexit (status ",
        "log_damaged_files": "Lima laesa vel deperdita inventa sunt:\n",
        "log_index_fail": "Nulla singula lima laesa inventa sunt, sed verificatio indicis defecit. Archivum PAR3 laesum esse potest.\n",
        "verify_title": "Verificare & Instaurare Volumina...",
        "label_vol_directory": "Directorium Voluminis:",
        "browse_btn": "Quaerere...",
        "select_vol_dir_title": "Elige Directorium Voluminis",
        "label_rec_directory": "Directorium Receptionis:",
        "select_rec_dir_title": "Elige Directorium Receptionis",
        "label_par3_base": "Nomen PAR3:",
        "log_frame_title_verify": "Acta Verificationis & Instaurationis",
        "close_btn": "Claudere",
        "verify_only_btn": "Solum Verificare",
        "restore_repair_btn": "Instaurare & Reparare",
        "iso_title": "Imago ISO Creare",
        "source_folder": "Directorium Fontis:",
        "target_iso_file": "Lima ISO Destinata:",
        "volume_label": "Titulus Voluminis:",
        "execution_log": "Acta Executionis",
        "generate_btn": "Creare",
        "complete_list_src_folders": "Index plenus directoriorum fontis:",
        "add_btn": "Addere...",
        "edit_btn": "Mutare...",
        "remove_btn": "Removere",
        "cancel_btn": "Dimittere",
        "ok_btn": "OK",
        "app_title": "Burn to the Brim",
        "app_version": "Versio 4.6.0",
        "app_copyright": "Copyright © 2001-2026 Sander Raaijmakers, Elwin Oost et societas Burn to the Brim",
        "select_medium_label": "Elige Medium:",
        "auto_size": "Mensura Automatica",
        "custom_size": "Mensura Commutabilis",
        "capacity_label": "Capacitas:",
        "save_cv_label": "Servare Vol. Pers.:",
        "custom_vol_name_placeholder": "Nomen voluminis personalis",
        "save_btn": "Servare",
        "cluster_label": "Magnitudo Cluster (Bytes):",
        "slack_label": "Bytes Slack:",
        "search_time_label": "Tempus Max Spatii (sec):",
        "split_depth_label": "Altiter Scindendi Dir:",
        "skip_empty_chk": "Omittere diaria / lima vacua",
        "skip_unreadable_chk": "Omittere lima non legenda (Clementer)",
        "integrate_ctx_chk": "Integrare in Windows Explorer contextu menu",
        "rule_wins_chk": "Ordinatio ex regulis vincit:",
        "enable_par3_chk": "Aptare PAR3 protectionem:",
        "par3_block_size": "PAR3 Magnitudo Block (Bytes):",
        "par3_redundancy": "PAR3 Redundantia Percent (%):",
        "language_label": "Lingua:",
        "grouping_rules_frame": "Regulae ordinandi lima / diaria",
        "pattern_col": "Muster",
        "files_col": "Lima",
        "folders_col": "Diaria",
        "type_col": "Typus",
        "rule_pattern_placeholder": "Regula (*.mp3 vel ^[0-9].*\\..*$)",
        "match_files_chk": "Lima Aequare",
        "match_folders_chk": "Diaria Aequare",
        "regex_pattern_chk": "Regex Pattern",
        "add_rule_btn": "Addere Regulam",
        "remove_selected_btn": "Removere Selecta",
        "restart_notice": "Praeferentiae linguae servatae sunt. Quaeso restart Burn to the Brim ut mutationes adhibeas.",
        "restart_title": "Lingua Mutata",
        "dir_setup_group": "Configuratio Directoriorum",
        "target_folder": "Directorium Destinatum:",
        "semantic_prompt": "Prompt Semanticus:",
        "move_files_chk": "Movere diaria/lima aptata in directorium destinatum",
        "create_symlinks_chk": "Creare nexus symbolicos in directorio destinato (Defalta)",
        "span_chk": "Diffundere per plura volumina (Volume_1, Volume_2, etc.)",
        "trace_chk": "Licere diagnosticum solvendi tracing (Trace)",
        "progress_frame_title": "Capacitas medii impleti",
        "filled_label": "Impletum: 0.00%",
        "log_frame_title": "Acta status et solventis",
        "time_left_label": "Tempus Reliquum: --:--",
        "test_btn": "Probate",
        "start_btn": "Incipere",
        "stop_btn": "Sistere",
        "pref_title": "Praeferentiae",
        "create_iso_btn": "Creare ISO...",
        "help_btn": "Auxilium",
        "about_btn": "De...",
        "import_json_btn": "Importare JSON...",
        "verify_restore_btn": "Verificare & Instaurare...",
        "tooltip_import_json": "Importare et legere offline indices JSON ad videnda volumina soluta.",
        "tooltip_verify_restore": "Verificare volumina soluta vel reparare laesa lima per PAR3.",
        "tooltip_test": "Simulare ordinationem sine limis scribendis ad tentandum volumen.",
        "tooltip_start": "Solvere et ordinare lima secundum praeferentias.",
        "tooltip_stop": "Sistere solutionem currentem vel transitionem.",
        "tooltip_create_iso": "Creare ISO filesystem imagines de voluminibus solutis.",
        "tooltip_source": "Semicoletis separata itinera directoriorum ad compingendum.",
        "tooltip_target": "Iter destinatum quo volumina ordinata scribentur.",
        "tooltip_semantic": "Scribe promptum sicut 'keep audio together' ad explicanda lima per MiniLM.",
        "tooltip_move": "Movere ipsa lima in folder voluminis destinati.",
        "tooltip_symlink": "Creare nexus symbolicos in folder voluminis destinati.",
        "tooltip_span": "Sinite diffundi per plura folder sequenter nominata.",
        "tooltip_trace": "Monstrare diagnosticum verbosissimum in actis.",
        "manage_src_folders_title": "Ordinare Fontis Diaria",
        "help_guide_title": "Burn to the Brim (BTTB) Manuale Auxilii",
        "help_title": "Auxilium - Burn to the Brim",
        "about_title": "De Burn to the Brim",
        "media_size_label": "Mensura Medii:",
        "save_cv_tooltip": "Servare currentem capacitatem, sectorem et slack ut nominatum volumen personalis",
        "skip_empty_label": "Omittere diaria/lima vacua:",
        "dark_theme_chk": "Licere Tenebrosum Thema:",
        "add_src_dir_title": "Addere directorium fontis",
        "edit_src_dir_title": "Mutare directorium fontis",
        "start_tooltip": "Incipere ordinare lima in optima volumina",
        "test_tooltip": "Simulare ordinationem et computare metrics sine modificatione in disco",
        "stop_tooltip": "Sistere solventem currentem vel copiam",
        "rel_path_col": "Relativum iter / Categoria",
        "size_col": "Magnitudo",
        "fitted_status_col": "Status aptationis",
        "source_tooltip": "Directorium fontis continens lima ordinanda",
        "browse_src_tooltip": "Quaerere directorium fontis",
        "target_tooltip": "Directorium destinatum ubi volumina ordinata creabuntur",
        "browse_dest_tooltip": "Quaerere directorium destinatum",
        "select_dest_dir_title": "Elige Directorium Destinatum",
        "semantic_placeholder": "ex. serva similes res una",
        "semantic_tooltip": "Specificare descriptionem semanticam ad colligenda similia lima per MiniLM AI",
        "move_tooltip": "Movere lima in volumina destinata loco exscribendi vel nectendi",
        "symlink_tooltip": "Creare nexus symbolicos pro exscribendo",
        "span_tooltip": "Aptare reliqua lima in plura volumina deinceps",
        "trace_tooltip": "Monstrare diagnostica et metrics in actis",
        "capacity_recommend_prompt_gtk": "Maximum scannatum postulat capacitatem saltem %.2f MB.\n\nElige actionem:",
        "btn_resize": "Mutare Mensuram",
        "btn_skip": "Omittere Lima",
        "btn_cancel": "Dimittere",
        "log_warn_dir_unreadable_1": "Cave: Directorium '",
        "log_warn_dir_unreadable_2": "' non legi potest (",
        "log_scan_aborted_unreadable_dir": "Scanning ab user ob non legibile directorium intermissum est.",
        "log_warn_item_unreadable_1": "Cave: Lima vel folder in '",
        "log_warn_item_unreadable_2": "' non legi potest (",
        "log_scan_aborted_unreadable_item": "Scanning ab user ob non legibile item intermissum est.",
        "log_warn_failed_iterate_dir_1": "Cave: Defecit iterare directorium '",
        "log_warn_failed_iterate_dir_2": "' (",
        "log_scan_aborted_failed_iterate": "Scanning ab user ob errorem itineris intermissum est.",
        "log_scanning_folder": "Scanning folder: ",
        "log_found_items": "Items inventa ad aptandum: ",
        "log_search_time_limit_exceeded_1": "Tempus spatii excessum (",
        "log_search_time_limit_exceeded_2": " sec).",
        "log_new_best_space_utilization": "Nova optima spatii utilitas: ",
        "log_selection_within_slack_1": "Selectio intra slack tolerantiam (",
        "log_selection_within_slack_2": "%) inventa. Terminans maturius.",
        "log_no_items_to_fit": "Nulla items ad aptandum.",
        "log_auto_volume_cap_set_1": "Auto Volumen capacitas posita ad: ",
        "log_auto_volume_cap_set_2": " bytes (",
        "log_warn_item_exceeds_cap_1": "Cave: Item '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " bytes) maius est quam volumen destinatum (",
        "log_capacity_adapted_1": "Capacitas adaptata ad ",
        "log_capacity_adapted_2": " bytes. Solventem iterando...",
        "log_capacity_skip_larger": "Maiora lima omissa. Solventem iterando...",
        "log_solver_aborted_files_exceed_cap": "Defecit solvens: dataset continet lima maiora capacitate.",
        "log_solver_aborted_files_exceed_cap_adapt": "Defecit solvens: dataset continet lima maiora capacitate. Apta capacitatem vel permitte iterare.",
        "log_volume_packing_estimation_header": "--- Voluminis Compingendi Aestimatio ---",
        "log_total_dataset_size_1": "Dataset magnitudo tota: ",
        "log_total_dataset_size_2": " bytes (",
        "log_target_volume_cap_1": "Volumen destinatum capacitas: ",
        "log_target_volume_cap_2": " bytes (",
        "log_theoretical_min_vols": "Volumina minima theoretice requisita: ",
        "log_medium_capacity_1": "Capacitas Medii: ",
        "log_medium_capacity_2": " sectores (",
        "log_solving_for_vol_header_1": "\n--- SOLVENDUM PRO VOLUMINE ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "Solvens optimam selectionem...",
        "log_finished_solving_1": "Solutio peracta. Tempus: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "Solutio unius voluminis: cogere omnia lima ad 100% inclusionem...",
        "log_backtracking_failed_fallback_greedy": "Backtracking solvens non invenit aptationem (vel exspiravit). Recadens ad greedy allocationem ut omnia reliqua lima includantur...",
        "log_forcing_progress_exceeds_cap_1": "Cogere progressum: compingens item reliquum maius medio (",
        "log_optimal_selection_covers": "Optima selectio tegit: ",
        "log_selected_items_for_volume_1": "Selecta items pro Volume ",
        "log_selected_items": "Selecta items:",
        "log_bytes_suffix": " bytes)",
        "log_organizing_item": "Ordinans item: ",
        "log_failed_symlink_retry_copy_1": "Defecit creare nexum symbolicum pro ",
        "log_failed_symlink_retry_copy_2": " (iterando cum copia)",
        "log_failed_organize_item_1": "Defecit ordinare ",
        "log_generating_par3_for_volume_1": "Creando PAR3 files pro Volume ",
        "log_success_par3_for_volume_1": "Prospere creata PAR3 protectio pro Volume ",
        "log_failed_par3_creation": "PAR3 creatio defecit: ",
        "log_test_simulation_complete": "--- SIMULATIO PROBATIONIS PERACTA ---",
        "log_no_files_written_on_disk": "Nulla lima scripta, exscripta vel mota in disco.",
        "log_completed_file_organization": "Ordinatio limorum peracta.",
        "log_created_offline_json_index": "Creatus est index JSON offline: ",
        "log_failed_create_json_index": "Defecit creare index JSON: ",
        "log_generating_semantic_embeddings": "Generando semanticos embeddings...",
        "log_failed_write_temp_embed_input": "Defecit scribere temp file pro embeddings: ",
        "log_warn_python_embed_engine_failed": "Cave: Python embed defectus vel python3 non est installed.",
        "log_semantic_grouping_fallback": "-> Semanticum raggruppamento cadet in local metrics.",
        "log_failed_read_temp_embed_output": "Defecit legere temp output file pro embeddings: ",
        "log_warn_embedded_output_empty": "Cave: Embed output vacuum est.",
        "log_running_semantic_clustering": "Clustering semanticum currens (threshold=0.6)...",
        "log_semantic_clustering_completed": "Clustering semanticum completum. Tota consolida: ",
        "about_comments": (
            "Burn to the Brim (BTTB) est translatio hodierna in C++20 priscae Delphi applicationis, facta ad optime aptanda lima et diaria in media destinata (CDs, DVDs, Blu-rays, vel USBs).\n\n"
            "Proprietates in v4.6.0:\n"
            "- Novissimum applicationis symbolum altae resolutionis (bttb.ico) et logo website unitum (bttb.png)\n"
            "- Minorati stack frames in investigatione & 16MB Win32 stack limit (removendo 0xC00000FD overflows)\n"
            "- Limites buffer scribendi extensi ad 10MB ne acta truncentur\n"
            "- Incremental GUI non-blocking rendering & skip capacitatum admonitus\n"
            "- Offiline indices JSON et interpretator interactivus\n"
            "- Optiva creatio et verificatio PAR3 filorum paritatis\n"
            "- Emendatio bit-perfecta per PAR3 copiam\n"
            "- Themata apta, inter quae tenebrosum thema in Linux GTK4\n"
            "- Melior computatio temporis reliqui statim ab initio\n"
            "- Custom Volume profiles nominata & dynamic auto volume sizing\n"
            "- Memoria configurationis ultimae selectae\n"
            "- Ordinatio ex regulis vel ex semanticis praevalens in conflictibus\n"
            "- Tempus reliquum aestimatum ex transitione et spinner status\n"
            "- Semantic Packing per MiniLM neural embeddings\n"
            "- Integrare in Windows Explorer contextu menu & viae longae legi possunt\n\n"
            "BTTB plene redditur et dynamic translate totam GUI per gettext .po in de, nl, fr, es.\n\n"
            "Bibliothecae et Attributiones:\n"
            "- libpar3 (a Yutaka Sawada, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3 (a BLAKE3 team, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS (a Christopher A. Taylor, BSD 3-Clause): https://github.com/catid/leopard\n"
            "- Galois Field library (a James S. Plank): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "Auctores: Sander Raaijmakers, Elwin Oost et Burn to the Brim societas"
        ),
        "help_guide_text": (
            "1. Altitudo Scindendi Directoriorum\n"
            "Decernit ad quam altitudinem folder res scindantur:\n"
            " - Altitudo 0 (Defalta): Suprema lima et folders ut singulae res tractantur.\n"
            " - Altitudo 1: Scindere uno gradu profundius fit, integra suprema folder tenens sed subfolders eorum scindens.\n\n"
            "2. Tempus Maximi Spatii\n"
            "Maximi secundi liciti ad solventem currere. Si perventum erit, optima electio tunc inventa adhibebitur.\n\n"
            "3. Spanning Slack\n"
            "Sinit solventem maturius finire si volumen impletum sit intra hos bytes ad capacitatem maximam (ex. 2048 bytes).\n\n"
            "4. Regulae ordinandi lima/diaria\n"
            "Coge matching res ut una in eodem volumine maneant (ex. congruens '*.mp3' vel regex '^album_.*').\n\n"
            "5. Fontis Diaria Plura (+)\n"
            "Preme '+' ad addenda fontis diaria plura. BTTB agit quasi omnia in uno radice folder essent.\n\n"
            "6. Nexus Symbolicos Creare\n"
            "Loco limis exscribendis vel movendis in folder destinatum, BTTB nexus symbolicos leves creat ad lima originalia.\n\n"
            "7. Neural Semantic Packing & MiniLM Setup Guide\n"
            "Per promptum semanticum, BTTB res similes colligit per deep learning embeddings.\n"
            "Ad hoc neuronal model MiniLM utendum, Python 3 et sentence-transformers installare oportet:\n"
            " - Step 1: Python 3 & pip installata esse cura.\n"
            "   (Linux: run 'sudo apt install python3 python3-pip python3-venv')\n"
            "   (Windows: Install ex https://www.python.org/ et check 'Add Python to PATH')\n"
            " - Step 2: Install sentence-transformers per terminalem:\n"
            "   Option A (Simplex):\n"
            "     pip install sentence-transformers\n"
            "   Option B (Virtual environment):\n"
            "     python3 -m venv bttb_env\n"
            "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
            "     pip install sentence-transformers\n"
            " - Step 3: Restart Burn to the Brim! Nisi inveniatur, BTTB cadet in character TF-IDF projector."
        )
    }

    # Now let's define Portuguese (pt)
    translations["pt"] = {
        "log_start_restore": "Iniciando a restauração/reparação do volume...\n",
        "log_volume_path": "Caminho do volume: ",
        "log_recovery_path": "Caminho de recuperação: ",
        "log_par3_base": "Nome base PAR3: ",
        "log_copying_repair": "Copiando conteúdos do volume e executando a reparação. Aguarde...\n",
        "log_errors": "Logs/Erros: ",
        "log_success_restore": "\nSUCESSO: Volume copiado e reparado com sucesso numa pasta de recuperação separada!\n",
        "log_fail_restore": "\nFALHA: A restauração ou reparação falhou.\n",
        "log_start_verify": "Iniciando a verificação do volume...\n",
        "log_success_verify": "\nSUCESSO: Todos os arquivos foram verificados e estão limpos/bit-perfect!\n",
        "log_fail_verify_status": "\nA verificação detetou problemas (status ",
        "log_damaged_files": "Arquivos danificados ou em falta encontrados:\n",
        "log_index_fail": "Nenhum arquivo danificado individual foi identificado, mas a verificação do índice falhou. O arquivo PAR3 pode estar danificado.\n",
        "verify_title": "Verificar & Restaurar Volumes...",
        "label_vol_directory": "Diretório do Volume:",
        "browse_btn": "Procurar...",
        "select_vol_dir_title": "Selecionar Diretório do Volume",
        "label_rec_directory": "Diretório de Recuperação:",
        "select_rec_dir_title": "Selecionar Diretório de Recuperação",
        "label_par3_base": "Nome Base PAR3:",
        "log_frame_title_verify": "Log de Verificação e Restauração",
        "close_btn": "Fechar",
        "verify_only_btn": "Apenas Verificar",
        "restore_repair_btn": "Restaurar & Reparar",
        "iso_title": "Criar Imagem ISO",
        "source_folder": "Pasta de Origem:",
        "target_iso_file": "Arquivo ISO de Destino:",
        "volume_label": "Etiqueta do Volume:",
        "execution_log": "Log de Execução",
        "generate_btn": "Gerar",
        "complete_list_src_folders": "Lista completa de pastas de origem:",
        "add_btn": "Adicionar...",
        "edit_btn": "Editar...",
        "remove_btn": "Remover",
        "cancel_btn": "Cancelar",
        "ok_btn": "OK",
        "app_title": "Burn to the Brim",
        "app_version": "Versão 4.6.0",
        "app_copyright": "Copyright © 2001-2026 Sander Raaijmakers, Elwin Oost e a equipe do Burn to the Brim",
        "select_medium_label": "Selecionar Mídia:",
        "auto_size": "Tamanho Automático",
        "custom_size": "Tamanho Personalizado",
        "capacity_label": "Capacidade:",
        "save_cv_label": "Salvar Volume Pers.:",
        "custom_vol_name_placeholder": "Nome do volume personalizado",
        "save_btn": "Salvar",
        "cluster_label": "Tamanho Cluster (Bytes):",
        "slack_label": "Bytes de Slack:",
        "search_time_label": "Tempo Max de Busca (seg):",
        "split_depth_label": "Profundidade de divisão dir:",
        "skip_empty_chk": "Ignorar pastas / arquivos vazios",
        "skip_unreadable_chk": "Ignorar arquivos ilegíveis (Tolerante)",
        "integrate_ctx_chk": "Integrar com o menu de contexto do Windows Explorer",
        "rule_wins_chk": "O agrupamento baseado em regras vence:",
        "enable_par3_chk": "Habilitar proteção de paridade PAR3:",
        "par3_block_size": "Tamanho do bloco PAR3 (Bytes):",
        "par3_redundancy": "Percentagem de redundância PAR3 (%):",
        "language_label": "Idioma:",
        "grouping_rules_frame": "Regras de agrupamento de arquivos / pastas",
        "pattern_col": "Padrão",
        "files_col": "Arquivos",
        "folders_col": "Pastas",
        "type_col": "Tipo",
        "rule_pattern_placeholder": "Padrão da regra (*.mp3 ou ^[0-9].*\\..*$)",
        "match_files_chk": "Corresponder Arquivos",
        "match_folders_chk": "Corresponder Pastas",
        "regex_pattern_chk": "Padrão Regex",
        "add_rule_btn": "Adicionar Regra",
        "remove_selected_btn": "Remover Selecionados",
        "restart_notice": "Preferências de idioma salvas. Reinicie o Burn to the Brim para aplicar as alterações.",
        "restart_title": "Idioma Alterado",
        "dir_setup_group": "Configuração de Diretórios",
        "target_folder": "Pasta de Destino:",
        "semantic_prompt": "Prompt semântico:",
        "move_files_chk": "Mover pastas/arquivos ajustados para a pasta de destino",
        "create_symlinks_chk": "Criar links simbólicos na pasta de destino (Padrão)",
        "span_chk": "Dividir em múltiplos volumes (Volume_1, Volume_2, etc.)",
        "trace_chk": "Habilitar registro detalhado de diagnóstico do solucionador (Trace)",
        "progress_frame_title": "Capacidade de mídia ajustada",
        "filled_label": "Preenchido: 0.00%",
        "log_frame_title": "Registros de status e solucionador",
        "time_left_label": "Tempo Restante: --:--",
        "test_btn": "Testar",
        "start_btn": "Iniciar",
        "stop_btn": "Parar",
        "pref_title": "Preferências",
        "create_iso_btn": "Criar ISO...",
        "help_btn": "Ajuda",
        "about_btn": "Sobre...",
        "import_json_btn": "Importar JSON...",
        "verify_restore_btn": "Verificar & Restaurar...",
        "tooltip_import_json": "Importar e analisar arquivos de índice JSON offline para visualizar detalhes do volume resolvido.",
        "tooltip_verify_restore": "Verificar volumes resolvidos ou copiar e restaurar arquivos danificados usando arquivos PAR3.",
        "tooltip_test": "Simular o empacotamento sem realizar operações nos arquivos no disco para testar a utilização.",
        "tooltip_start": "Executar o solucionador e organizar arquivos/pastas de acordo com as preferências.",
        "tooltip_stop": "Abortar o processo atual de empacotamento ou organização de arquivos.",
        "tooltip_create_iso": "Criar imagens do sistema de arquivos ISO a partir dos volumes resolvidos.",
        "tooltip_source": "Caminhos das pastas a serem empacotadas e consolidadas, separados por ponto e vírgula.",
        "tooltip_target": "Caminho de destino onde os volumes serão organizados e escritos.",
        "tooltip_semantic": "Introduza um prompt como 'keep audio together' para analisar arquivos através do MiniLM.",
        "tooltip_move": "Mover/cortar os arquivos/pastas originais diretamente para as pastas de destino do volume.",
        "tooltip_symlink": "Criar links simbólicos do sistema de arquivos nas pastas de destino do volume (não destrutivo).",
        "tooltip_span": "Permitir a divisão de conteúdos em múltiplas pastas nomeadas sequencialmente.",
        "tooltip_trace": "Exibir diagnósticos detalhados e métricas de desempenho nos registros.",
        "manage_src_folders_title": "Gerenciar pastas de origem",
        "help_guide_title": "Guia de Ajuda do Burn to the Brim (BTTB)",
        "help_title": "Ajuda - Burn to the Brim",
        "about_title": "Sobre o Burn to the Brim",
        "media_size_label": "Tamanho da Mídia:",
        "save_cv_tooltip": "Salvar capacidade, setor e slack atuais como um volume personalizado nomeado",
        "skip_empty_label": "Ignorar Arquivos/Pastas vazios:",
        "dark_theme_chk": "Habilitar Tema Escuro:",
        "add_src_dir_title": "Adicionar diretório de origem",
        "edit_src_dir_title": "Editar diretório de origem",
        "start_tooltip": "Iniciar a organização dos arquivos em volumes ideais",
        "test_tooltip": "Simular a organização dos arquivos e calcular métricas sem modificar arquivos no disco",
        "stop_tooltip": "Parar o solucionador atual ou operação de cópia",
        "rel_path_col": "Caminho relativo / Categoria",
        "size_col": "Tamanho",
        "fitted_status_col": "Status do ajuste",
        "source_tooltip": "Diretório de origem contendo os arquivos a serem organizados",
        "browse_src_tooltip": "Procurar diretório de origem",
        "target_tooltip": "Diretório de destino onde os volumes organizados serão criados",
        "browse_dest_tooltip": "Procurar diretório de destino",
        "select_dest_dir_title": "Selecionar diretório de destino",
        "semantic_placeholder": "ex. manter conteúdos semelhantes juntos",
        "semantic_tooltip": "Especificar uma descrição semântica para agrupar arquivos semelhantes usando MiniLM AI",
        "move_tooltip": "Mover arquivos para seus volumes de destino em vez de copiar ou criar links simbólicos",
        "symlink_tooltip": "Criar links simbólicos dos arquivos no destino em vez de os copiar",
        "span_tooltip": "Ajustar arquivos restantes em múltiplos volumes sequencialmente em vez de apenas no primeiro volume",
        "trace_tooltip": "Exibir diagnósticos detalhados e métricas de desempenho nos registros",
        "capacity_recommend_prompt_gtk": "O maior item escaneado requer uma capacidade de volume de pelo menos %.2f MB.\n\nEscolha uma ação:",
        "btn_resize": "Redimensionar",
        "btn_skip": "Ignorar arquivos",
        "btn_cancel": "Cancelar",
        "log_warn_dir_unreadable_1": "Aviso: Diretório '",
        "log_warn_dir_unreadable_2": "' não legível (",
        "log_scan_aborted_unreadable_dir": "Varredura abortada pelo usuário devido a diretório ilegível.",
        "log_warn_item_unreadable_1": "Aviso: Arquivo ou pasta em '",
        "log_warn_item_unreadable_2": "' não legível (",
        "log_scan_aborted_unreadable_item": "Varredura abortada pelo usuário devido a item ilegível.",
        "log_warn_failed_iterate_dir_1": "Aviso: Falha ao iterar o diretório '",
        "log_warn_failed_iterate_dir_2": "' (",
        "log_scan_aborted_failed_iterate": "Varredura abortada pelo usuário devido a falha ao iterar o diretório.",
        "log_scanning_folder": "Escaneando pasta: ",
        "log_found_items": "Itens encontrados para ajuste: ",
        "log_search_time_limit_exceeded_1": "Limite de tempo de busca excedido (",
        "log_search_time_limit_exceeded_2": " segundos).",
        "log_new_best_space_utilization": "Nova melhor utilização de espaço: ",
        "log_selection_within_slack_1": "Seleção dentro da tolerância de slack (",
        "log_selection_within_slack_2": "%) encontrada. Terminando busca antecipadamente.",
        "log_no_items_to_fit": "Nenhum item para ajustar.",
        "log_auto_volume_cap_set_1": "Capacidade de auto volume configurada para: ",
        "log_auto_volume_cap_set_2": " bytes (",
        "log_warn_item_exceeds_cap_1": "Aviso: Item '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " bytes) é maior do que o tamanho do volume de destino (",
        "log_capacity_adapted_1": "Capacidade adaptada para ",
        "log_capacity_adapted_2": " bytes. Tentando solucionador novamente...",
        "log_capacity_skip_larger": "Arquivos maiores ignorados. Tentando solucionador novamente...",
        "log_solver_aborted_files_exceed_cap": "Solucionador abortado: o conjunto de dados contém arquivos que excedem a capacidade.",
        "log_solver_aborted_files_exceed_cap_adapt": "Solucionador abortado: o conjunto de dados contém arquivos que excedem a capacidade. Adapte a capacidade ou ative a tentativa interativa.",
        "log_volume_packing_estimation_header": "--- Estimação de Empacotamento de Volume ---",
        "log_total_dataset_size_1": "Tamanho total do conjunto de dados: ",
        "log_total_dataset_size_2": " bytes (",
        "log_target_volume_cap_1": "Capacidade do volume de destino: ",
        "log_target_volume_cap_2": " bytes (",
        "log_theoretical_min_vols": "Volumes mínimos teóricos necessários: ",
        "log_medium_capacity_1": "Capacidade do Meio: ",
        "log_medium_capacity_2": " setores (",
        "log_solving_for_vol_header_1": "\r\n--- SOLUCIONANDO PARA O VOLUME ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "Solucionando seleção ideal de bins...",
        "log_finished_solving_1": "Solução concluída. Tempo decorrido: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "Solução de volume único: forçando seleção de todos os arquivos para garantir 100% de inclusão...",
        "log_backtracking_failed_fallback_greedy": "O solucionador com backtracking não encontrou um ajuste (ou expirou). Recorrendo à alocação gananciosa para garantir que todos os arquivos restantes sejam incluídos...",
        "log_forcing_progress_exceeds_cap_1": "Forçando progresso: empacotando o item restante maior que a capacidade da mídia (",
        "log_optimal_selection_covers": "A seleção ideal cobre: ",
        "log_selected_items_for_volume_1": "Itens selecionados para o Volume ",
        "log_selected_items": "Itens selecionados:",
        "log_bytes_suffix": " bytes)",
        "log_organizing_item": "Organizando item: ",
        "log_failed_symlink_retry_copy_1": "Falha ao criar link simbólico para ",
        "log_failed_symlink_retry_copy_2": " (tentando novamente com cópia)",
        "log_failed_organize_item_1": "Falha ao organizar ",
        "log_generating_par3_for_volume_1": "Gerando arquivos de paridade PAR3 para o Volume ",
        "log_success_par3_for_volume_1": "Criada com sucesso a proteção de paridade PAR3 para o Volume ",
        "log_failed_par3_creation": "A criação do PAR3 falhou: ",
        "log_test_simulation_complete": "--- SIMULAÇÃO DE TESTE CONCLUÍDA ---",
        "log_no_files_written_on_disk": "Nenhum arquivo foi copiado, linkado simbolicamente ou movido no disco.",
        "log_completed_file_organization": "Organização de arquivos concluída.",
        "log_created_offline_json_index": "Arquivo de índice JSON offline criado: ",
        "log_failed_create_json_index": "Falha ao criar arquivo de índice JSON: ",
        "log_generating_semantic_embeddings": "Gerando embeddings semânticos...",
        "log_failed_write_temp_embed_input": "Falha ao gravar arquivo de entrada de embedding temporário: ",
        "log_warn_python_embed_engine_failed": "Aviso: O mecanismo de embedding python do subprocesso falhou ou o python3 não está instalado.",
        "log_semantic_grouping_fallback": "-> O agrupamento semântico recorrerá a métricas de correspondência de strings locais.",
        "log_failed_read_temp_embed_output": "Falha ao ler arquivo de saída de embedding temporário: ",
        "log_warn_embedded_output_empty": "Aviso: O output incorporado está vazio.",
        "log_running_semantic_clustering": "Executando agrupamento semântico aglomerativo (limite=0.6)...",
        "log_semantic_clustering_completed": "Agrupamento semântico concluído. Total de grupos consolidados: ",
        "about_comments": (
            "O Burn to the Brim (BTTB) é uma conversão moderna em C++20 da clássica aplicação Delphi, projetada para ajustar de forma ideal arquivos e pastas em mídias de armazenamento de destino (CDs, DVDs, Blu-rays ou USBs).\n\n"
            "Recursos na v4.6.0:\n"
            "- Ícone da aplicação de alta resolução totalmente novo (bttb.ico) e logotipo unificado do site (bttb.png)\n"
            "- Minimização dos stack frames do estado de pesquisa & limite de stack Win32 de 16MB (corrigindo estouros 0xC00000FD)\n"
            "- Limites de buffer de log estendidos para 10MB para evitar a exclusão do log de rastreamento\n"
            "- Renderização de GUI incremental não bloqueante & exclusão de avisos de capacidade de arquivos\n"
            "- Criação de índice JSON offline e analisador interativo\n"
            "- Geração e verificação opcional de arquivos de paridade PAR3\n"
            "- Restauração e reparo baseados em cópia PAR3 bit-perfect\n"
            "- Suporte a temas, incluindo opções padrão de tema escuro no Linux GTK4\n"
            "- Cálculo aprimorado do Tempo Restante Estimado imediatamente na inicialização\n"
            "- Perfis de volume personalizados nomeados & dimensionamento dinâmico de volume automático\n"
            "- Memória das configurações que restaura a última configuração de volume selecionada\n"
            "- Substituições de conflito de regras permitindo que o agrupamento baseado em regras ou semântico vença\n"
            "- Tempo restante estimado baseado na taxa de transferência & indicador spinner de atividade de status\n"
            "- Empacotamento semântico ciente da entropia baseado em embeddings MiniLM\n"
            "- Integração do menu de contexto do Windows Explorer & suporte a caminhos longos\n\n"
            "O BTTB está totalmente localizado e traduz dinamicamente toda a interface do usuário com base nos modelos gettext .po padrão em alemão, holandês, francês e espanhol.\n\n"
            "Bibliotecas e Atribuições Utilizadas:\n"
            "- libpar3 (por Yutaka Sawada, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3 (pela equipe BLAKE3, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS (por Christopher A. Taylor, BSD 3-Clause): https://github.com/catid/leopard\n"
            "- Biblioteca Galois Field (por James S. Plank): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "Autores: Sander Raaijmakers, Elwin Oost e a equipe do Burn to the Brim"
        ),
        "help_guide_text": (
            "1. Profundidade de divisão de diretório\n"
            "Determina o nível de aninhamento de pastas no qual os itens são divididos:\n"
            " - Profundidade 0 (Padrão): os arquivos e pastas de nível superior são tratados como itens separados.\n"
            " - Profundidade 1: a divisão ocorre um nível mais abaixo, mantendo intactas as pastas de nível superior, mas dividindo as suas subpastas imediatas.\n\n"
            "2. Tempo máximo de busca\n"
            "O máximo de segundos que o solucionador com backtracking tem permissão para rodar. Se atingido, a melhor seleção encontrada até o momento é usada.\n\n"
            "3. Spanning Slack\n"
            "Permite a terminação antecipada do solucionador quando um volume estiver empacotado dentro deste número de bytes a partir da capacidade máxima absoluta (ex: 2048 bytes).\n\n"
            "4. Regras de agrupamento de arquivos/pastas\n"
            "Força os itens correspondentes a permanecerem agrupados no mesmo volume (ex: correspondente a '*.mp3' ou regex '^album_.*').\n\n"
            "5. Pastas de Origem Múltiplas (+)\n"
            "Clique em '+' para especificar várias pastas de origem. O BTTB age como se estivessem num único diretório raiz. Caminhos de origem aninhados são ignorados.\n\n"
            "6. Criar Links Simbólicos\n"
            "Em vez de copiar/mover os arquivos para a pasta de destino, o BTTB cria links simbólicos leves apontando para os seus arquivos originais.\n\n"
            "7. Empacotamento Semântico Neural & Guia de Configuração MiniLM\n"
            "Ao especificar um prompt semântico, o BTTB agrupa arquivos com conteúdo semelhante usando embeddings de deep learning cientes do contexto.\n"
            "Para usar o modelo neural MiniLM preferido de alta precisão, deve instalar o Python 3 e sentence-transformers:\n"
            " - Passo 1: garanta que o Python 3 e o pip estejam instalados.\n"
            "   (Linux: execute 'sudo apt install python3 python3-pip python3-venv')\n"
            "   (Windows: instale a partir de https://www.python.org/ e marque 'Add Python to PATH')\n"
            " - Passo 2: instale sentence-transformers via terminal ou prompt de comando:\n"
            "   Opção A (Recomendado pela simplicidade):\n"
            "     pip install sentence-transformers\n"
            "   Opção B (Isolamento de ambiente virtual):\n"
            "     python3 -m venv bttb_env\n"
            "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
            "     pip install sentence-transformers\n"
            " - Passo 3: reinicie o Burn to the Brim para carregar o MiniLM automaticamente! Se não for encontrado, o BTTB recorrerá graciosamente a um projetor TF-IDF de caracteres localizado."
        )
    }

    # Now let's define Hindi (hi)
    translations["hi"] = {
        "log_start_restore": "वॉल्यूम पुनर्प्राप्ति/मरम्मत शुरू हो रही है...\n",
        "log_volume_path": "वॉल्यूम पथ: ",
        "log_recovery_path": "पुनर्प्राप्ति पथ: ",
        "log_par3_base": "PAR3 मूल नाम: ",
        "log_copying_repair": "वॉल्यूम सामग्री की प्रतिलिपि बनाई जा रही है और मरम्मत शुरू की जा रही है। कृपया प्रतीक्षा करें...\n",
        "log_errors": "लॉग/त्रुटियां: ",
        "log_success_restore": "\nसफलता: वॉल्यूम सफलतापूर्वक कॉपी किया गया और अलग पुनर्प्राप्ति फ़ोल्डर में मरम्मत की गई!\n",
        "log_fail_restore": "\nविफलता: पुनर्प्राप्ति या मरम्मत विफल रही।\n",
        "log_start_verify": "वॉल्यूम सत्यापन शुरू हो रहा है...\n",
        "log_success_verify": "\nसफलता: सभी फाइलें सत्यापित हैं और स्वच्छ/बिट-परफेक्ट हैं!\n",
        "log_fail_verify_status": "\nसत्यापन में समस्याएं पाई गईं (स्थिति ",
        "log_damaged_files": "क्षतिग्रस्त या अनुपलब्ध फ़ाइलें मिलीं:\n",
        "log_index_fail": "कोई व्यक्तिगत क्षतिग्रस्त फ़ाइल नहीं मिली, लेकिन अनुक्रमणिका सत्यापन विफल रहा। PAR3 संग्रह स्वयं क्षतिग्रस्त हो सकता है।\n",
        "verify_title": "वॉल्यूम सत्यापित और पुनर्प्राप्त करें...",
        "label_vol_directory": "वॉल्यूम निर्देशिका:",
        "browse_btn": "ब्राउज़ करें...",
        "select_vol_dir_title": "वॉल्यूम निर्देशिका चुनें",
        "label_rec_directory": "पुनर्प्राप्ति निर्देशिका:",
        "select_rec_dir_title": "पुनर्प्राप्ति निर्देशिका चुनें",
        "label_par3_base": "PAR3 मूल नाम:",
        "log_frame_title_verify": "सत्यापन और पुनर्प्राप्ति लॉग",
        "close_btn": "बंद करें",
        "verify_only_btn": "केवल सत्यापित करें",
        "restore_repair_btn": "पुनर्प्राप्त और मरम्मत करें",
        "iso_title": "ISO छवि बनाएं",
        "source_folder": "स्रोत फ़ोल्डर:",
        "target_iso_file": "लक्षित ISO फ़ाइल:",
        "volume_label": "वॉल्यूम लेबल:",
        "execution_log": "निष्पादन लॉग",
        "generate_btn": "बनाएं",
        "complete_list_src_folders": "स्रोत फ़ोल्डर्स की पूरी सूची:",
        "add_btn": "जोड़ें...",
        "edit_btn": "संपादित करें...",
        "remove_btn": "हटाएं",
        "cancel_btn": "रद्द करें",
        "ok_btn": "ठीक है",
        "app_title": "Burn to the Brim",
        "app_version": "संस्करण 4.6.0",
        "app_copyright": "कॉपीराइट © 2001-2026 Sander Raaijmakers, Elwin Oost और Burn to the Brim टीम",
        "select_medium_label": "माध्यम चुनें:",
        "auto_size": "स्वचालित आकार",
        "custom_size": "कस्टम आकार",
        "capacity_label": "क्षमता:",
        "save_cv_label": "कस्टम वॉल्यूम सहेजें:",
        "custom_vol_name_placeholder": "कस्टम वॉल्यूम नाम",
        "save_btn": "सहेजें",
        "cluster_label": "क्लस्टर आकार (बाइट्स):",
        "slack_label": "स्लैक बाइट्स:",
        "search_time_label": "अधिकतम खोज समय (सेकंड):",
        "split_depth_label": "निर्देशिका विभाजन गहराई:",
        "skip_empty_chk": "खाली फ़ोल्डर्स / फाइलें छोड़ें",
        "skip_unreadable_chk": "अपठनीय फाइलें छोड़ें (सहानुभूतिपूर्वक)",
        "integrate_ctx_chk": "Windows Explorer राइट-क्लिक मेनू में एकीकृत करें",
        "rule_wins_chk": "नियम-आधारित समूहीकरण की जीत होती है:",
        "enable_par3_chk": "PAR3 पैरिटी सुरक्षा सक्षम करें:",
        "par3_block_size": "PAR3 ब्लॉक आकार (बाइट्स):",
        "par3_redundancy": "PAR3 अतिरेक प्रतिशत (%):",
        "language_label": "भाषा:",
        "grouping_rules_frame": "फ़ाइल / फ़ोल्डर समूहीकरण नियम",
        "pattern_col": "पैटर्न",
        "files_col": "फ़ाइलें",
        "folders_col": "फ़ोल्डर्स",
        "type_col": "प्रकार",
        "rule_pattern_placeholder": "नियम पैटर्न (*.mp3 या ^[0-9].*\\..*$)",
        "match_files_chk": "फ़ाइलें मिलान करें",
        "match_folders_chk": "फ़ोल्डर्स मिलान करें",
        "regex_pattern_chk": "Regex पैटर्न",
        "add_rule_btn": "नियम जोड़ें",
        "remove_selected_btn": "चयनित हटाएं",
        "restart_notice": "भाषा प्राथमिकताएं सहेजी गईं। परिवर्तनों को लागू करने के लिए कृपया Burn to the Brim को पुनरारंभ करें।",
        "restart_title": "भाषा बदली गई",
        "dir_setup_group": "निर्देशिका सेटअप",
        "target_folder": "लक्षित फ़ोल्डर:",
        "semantic_prompt": "सिमेंटिक प्रॉम्प्ट:",
        "move_files_chk": "फ़िट किए गए फ़ोल्डर्स/फ़ाइलों को लक्षित फ़ोल्डर में ले जाएं",
        "create_symlinks_chk": "लक्षित फ़ोल्डर में सिम्बॉलिक लिंक बनाएं (डिफ़ॉल्ट)",
        "span_chk": "एकाधिक वॉल्यूमों में विभाजित करें (Volume_1, Volume_2, आदि)",
        "trace_chk": "विस्तृत सॉल्वर नैदानिक अनुरेखण सक्षम करें (Trace)",
        "progress_frame_title": "फ़िट की गई माध्यम क्षमता",
        "filled_label": "भरा हुआ: 0.00%",
        "log_frame_title": "स्थिति और सॉल्वर लॉग",
        "time_left_label": "शेष समय: --:--",
        "test_btn": "परीक्षण",
        "start_btn": "प्रारंभ",
        "stop_btn": "रोकें",
        "pref_title": "प्राथमिकताएं",
        "create_iso_btn": "ISO बनाएं...",
        "help_btn": "सहायता",
        "about_btn": "के बारे में...",
        "import_json_btn": "JSON आयात करें...",
        "verify_restore_btn": "सत्यापित और पुनर्प्राप्त करें...",
        "tooltip_import_json": "सुलझाए गए वॉल्यूम फ़ाइल विवरण देखने के लिए ऑफ़लाइन JSON अनुक्रमणिका फ़ाइलें आयात और पार्स करें।",
        "tooltip_verify_restore": "सुलझाए गए वॉल्यूम सत्यापित करें या PAR3 संग्रहों का उपयोग करके क्षतिग्रस्त फ़ाइलों को कॉपी और पुनर्प्राप्त करें।",
        "tooltip_test": "वॉल्यूम उपयोग का परीक्षण करने के लिए डिस्क पर फ़ाइलों को संशोधित किए बिना पैकिंग का अनुकरण करें।",
        "tooltip_start": "सॉल्वर चलाएं और प्राथमिकताओं के अनुसार फ़ाइलों/फ़ोल्डरों को व्यवस्थित करें।",
        "tooltip_stop": "वर्तमान में चल रही पैकिंग या फ़ाइल संगठन प्रक्रिया को रोकें।",
        "tooltip_create_iso": "सुलझाए गए वॉल्यूम से ISO फ़ाइल सिस्टम छवियां बनाएं।",
        "tooltip_source": "पैक और समेकित करने के लिए फ़ोल्डरों के अर्धविराम-अलग पथ।",
        "tooltip_target": "लक्षित पथ जहाँ वॉल्यूम व्यवस्थित और लिखे जाएंगे।",
        "tooltip_semantic": "MiniLM के माध्यम से फ़ाइलों का विश्लेषण करने के लिए 'keep audio together' जैसा प्रॉम्प्ट दर्ज करें।",
        "tooltip_move": "मूल फ़ाइलों/फ़ोल्डरों को सीधे लक्षित वॉल्यूम फ़ोल्डरों में ले जाएं/काटें।",
        "tooltip_symlink": "लक्षित वॉल्यूम फ़ोल्डरों में फ़ाइल सिस्टम सिम्बॉलिक लिंक बनाएं (गैर-विनाशकारी)।",
        "tooltip_span": "सामग्री को क्रमिक रूप से नामित एकाधिक फ़ोल्डरों में विभाजित करने की अनुमति दें।",
        "tooltip_trace": "लॉग में विस्तृत निदान और प्रदर्शन मीट्रिक दिखाएं।",
        "manage_src_folders_title": "स्रोत फ़ोल्डर्स प्रबंधित करें",
        "help_guide_title": "Burn to the Brim (BTTB) सहायता गाइड",
        "help_title": "सहायता - Burn to the Brim",
        "about_title": "Burn to the Brim के बारे में",
        "media_size_label": "माध्यम का आकार:",
        "save_cv_tooltip": "वर्तमान क्षमता, सेक्टर और स्लैक को नाम देकर कस्टम वॉल्यूम के रूप में सहेजें",
        "skip_empty_label": "खाली फ़ाइलें/निर्देशिकाएं छोड़ें:",
        "dark_theme_chk": "डार्क थीम सक्षम करें:",
        "add_src_dir_title": "स्रोत निर्देशिका जोड़ें",
        "edit_src_dir_title": "स्रोत निर्देशिका संपादित करें",
        "start_tooltip": "फ़ाइलों को इष्टतम वॉल्यूमों में व्यवस्थित करना शुरू करें",
        "test_tooltip": "डिस्क पर फ़ाइलों को संशोधित किए बिना फ़ाइलों के संगठन का अनुकरण करें और मीट्रिक की गणना करें",
        "stop_tooltip": "वर्तमान सॉल्वर या कॉपी संचालन को रोकें",
        "rel_path_col": "सापेक्ष पथ / श्रेणी",
        "size_col": "आकार",
        "fitted_status_col": "फ़िट की गई स्थिति",
        "source_tooltip": "व्यवस्थित की जाने वाली फ़ाइलों वाली स्रोत निर्देशिका",
        "browse_src_tooltip": "स्रोत निर्देशिका खोजें",
        "target_tooltip": "लक्षित निर्देशिका जहां व्यवस्थित वॉल्यूम बनाए जाएंगे",
        "browse_dest_tooltip": "लक्षित निर्देशिका खोजें",
        "select_dest_dir_title": "लक्षित निर्देशिका चुनें",
        "semantic_placeholder": "जैसे समान सामग्री को एक साथ रखना",
        "semantic_tooltip": "MiniLM AI का उपयोग करके समान फ़ाइलों को क्लस्टर करने के लिए एक सिमेंटिक विवरण निर्दिष्ट करें",
        "move_tooltip": "कॉपी करने या सिम्बॉलिक लिंक बनाने के बजाय फ़ाइलों को उनके लक्षित वॉल्यूम में ले जाएं",
        "symlink_tooltip": "कॉपी करने के बजाय गंतव्य पर फ़ाइलों के सिम्बॉलिक लिंक बनाएं",
        "span_tooltip": "केवल पहले वॉल्यूम के बजाय शेष फ़ाइलों को क्रमिक रूप से कई वॉल्यूमों में फ़िट करें",
        "trace_tooltip": "लॉग में विस्तृत नैदानिक और प्रदर्शन मीट्रिक दिखाएं",
        "capacity_recommend_prompt_gtk": "सबसे बड़ी स्कैन की गई वस्तु के लिए कम से कम %.2f MB की वॉल्यूम क्षमता की आवश्यकता होती है।\n\nएक क्रिया चुनें:",
        "btn_resize": "आकार बदलें",
        "btn_skip": "फ़ाइलें छोड़ें",
        "btn_cancel": "रद्द करें",
        "log_warn_dir_unreadable_1": "चेतावनी: निर्देशिका '",
        "log_warn_dir_unreadable_2": "' अपठनीय है (",
        "log_scan_aborted_unreadable_dir": "अपठनीय निर्देशिका के कारण उपयोगकर्ता द्वारा स्कैनिंग रद्द कर दी गई।",
        "log_warn_item_unreadable_1": "चेतावनी: '",
        "log_warn_item_unreadable_2": "' में फ़ाइल या फ़ोल्डर अपठनीय है (",
        "log_scan_aborted_unreadable_item": "अपठनीय वस्तु के कारण उपयोगकर्ता द्वारा स्कैनिंग रद्द कर दी गई।",
        "log_warn_failed_iterate_dir_1": "चेतावनी: निर्देशिका '",
        "log_warn_failed_iterate_dir_2": "' को पुनरावृत्त करने में विफल (",
        "log_scan_aborted_failed_iterate": "निर्देशिका पुनरावृत्ति विफलता के कारण उपयोगकर्ता द्वारा स्कैनिंग रद्द कर दी गई।",
        "log_scanning_folder": "फ़ोल्डर स्कैन किया जा रहा है: ",
        "log_found_items": "फ़िट करने के लिए वस्तुएं मिलीं: ",
        "log_search_time_limit_exceeded_1": "खोज समय सीमा समाप्त (",
        "log_search_time_limit_exceeded_2": " सेकंड)।",
        "log_new_best_space_utilization": "नया सर्वोत्तम स्थान उपयोग: ",
        "log_selection_within_slack_1": "स्लैक सहिष्णुता के भीतर चयन (",
        "log_selection_within_slack_2": "%) मिला। खोज समय से पहले समाप्त कर दी गई।",
        "log_no_items_to_fit": "फ़िट करने के लिए कोई वस्तु नहीं है।",
        "log_auto_volume_cap_set_1": "ऑटो वॉल्यूम क्षमता इस प्रकार निर्धारित की गई: ",
        "log_auto_volume_cap_set_2": " बाइट्स (",
        "log_warn_item_exceeds_cap_1": "चेतावनी: वस्तु '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " बाइट्स) लक्षित वॉल्यूम आकार से बड़ी है (",
        "log_capacity_adapted_1": "क्षमता अनुकूलित की गई ",
        "log_capacity_adapted_2": " बाइट्स। सॉल्वर पुनः प्रयास कर रहा है...",
        "log_capacity_skip_larger": "बड़ी फाइलें छोड़ दी गईं। सॉल्वर पुनः प्रयास कर रहा है...",
        "log_solver_aborted_files_exceed_cap": "सॉल्वर निरस्त: डेटासेट में क्षमता से अधिक फाइलें हैं।",
        "log_solver_aborted_files_exceed_cap_adapt": "सॉल्वर निरस्त: डेटासेट में क्षमता से अधिक फाइलें हैं। क्षमता को अनुकूलित करें या इंटरैक्टिव पुनः प्रयास सक्षम करें।",
        "log_volume_packing_estimation_header": "--- वॉल्यूम पैकिंग अनुमान ---",
        "log_total_dataset_size_1": "कुल डेटासेट आकार: ",
        "log_total_dataset_size_2": " बाइट्स (",
        "log_target_volume_cap_1": "लक्षित वॉल्यूम क्षमता: ",
        "log_target_volume_cap_2": " बाइट्स (",
        "log_theoretical_min_vols": "सैद्धांतिक न्यूनतम वॉल्यूम आवश्यक: ",
        "log_medium_capacity_1": "माध्यम क्षमता: ",
        "log_medium_capacity_2": " सेक्टर (",
        "log_solving_for_vol_header_1": "\r\n--- वॉल्यूम के लिए समाधान खोज रहे हैं ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "इष्टतम बिन चयन का समाधान खोज रहे हैं...",
        "log_finished_solving_1": "समाधान खोजा गया। बीता समय: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "एकल वॉल्यूम समाधान: 100% समावेशन की गारंटी के लिए सभी फ़ाइलों के चयन को बाध्य करना...",
        "log_backtracking_failed_fallback_greedy": "बैकट्रैकिंग सॉल्वर को कोई उपयुक्तता नहीं मिली (या समय समाप्त हो गया)। यह सुनिश्चित करने के लिए कि सभी शेष फाइलें शामिल हों, लालची आवंटन का सहारा लिया जा रहा है...",
        "log_forcing_progress_exceeds_cap_1": "प्रगति बाध्य करना: माध्यम क्षमता से बड़ी शेष वस्तु को पैक किया जा रहा है (",
        "log_optimal_selection_covers": "इष्टतम चयन कवर करता है: ",
        "log_selected_items_for_volume_1": "वॉल्यूम के लिए चयनित वस्तुएं ",
        "log_selected_items": "चयनित वस्तुएं:",
        "log_bytes_suffix": " बाइट्स)",
        "log_organizing_item": "वस्तु को व्यवस्थित किया जा रहा है: ",
        "log_failed_symlink_retry_copy_1": "सिंबॉलिक लिंक बनाने में विफल ",
        "log_failed_symlink_retry_copy_2": " (कॉपी के साथ पुनः प्रयास कर रहे हैं)",
        "log_failed_organize_item_1": "व्यवस्थित करने में विफल ",
        "log_generating_par3_for_volume_1": "वॉल्यूम के लिए PAR3 पैरिटी फाइलें उत्पन्न की जा रही हैं ",
        "log_success_par3_for_volume_1": "वॉल्यूम के लिए सफलतापूर्वक PAR3 पैरिटी सुरक्षा बनाई गई ",
        "log_failed_par3_creation": "PAR3 निर्माण विफल: ",
        "log_test_simulation_complete": "--- परीक्षण अनुकरण पूरा हुआ ---",
        "log_no_files_written_on_disk": "डिस्क पर कोई फ़ाइल कॉपी, सिंबॉलिक लिंक या स्थानांतरित नहीं की गई थी।",
        "log_completed_file_organization": "फ़ाइल संगठन पूरा हुआ।",
        "log_created_offline_json_index": "ऑफ़लाइन JSON अनुक्रमणिका फ़ाइल बनाई गई: ",
        "log_failed_create_json_index": "JSON अनुक्रमणिका फ़ाइल बनाने में विफल: ",
        "log_generating_semantic_embeddings": "सिमेंटिक एम्बेडिंग उत्पन्न की जा रही है...",
        "log_failed_write_temp_embed_input": "अस्थायी एम्बेडिंग इनपुट फ़ाइल लिखने में विफल: ",
        "log_warn_python_embed_engine_failed": "चेतावनी: सबप्रोसेस पायथन एम्बेडिंग इंजन विफल या पायथन3 स्थापित नहीं है।",
        "log_semantic_grouping_fallback": "-> सिमेंटिक समूहीकरण स्थानीय स्ट्रिंग मिलान मीट्रिक्स पर वापस आ जाएगा।",
        "log_failed_read_temp_embed_output": "अस्थायी एम्बेडिंग आउटपुट फ़ाइल पढ़ने में विफल: ",
        "log_warn_embedded_output_empty": "चेतावनी: एम्बेडेड आउटपुट खाली है।",
        "log_running_semantic_clustering": "समूहीकृत सिमेंटिक क्लस्टरिंग चल रही है (सीमा=0.6)...",
        "log_semantic_clustering_completed": "सिमेंटिक क्लस्टरिंग पूरी हुई। कुल समेकित समूह: ",
        "about_comments": (
            "Burn to the Brim (BTTB) क्लासिक डेल्फी एप्लिकेशन का एक आधुनिक C++20 पोर्ट है जिसे लक्षित भंडारण माध्यमों (CDs, DVDs, Blu-rays, या USBs) पर फ़ाइलों और फ़ोल्डरों को इष्टतम रूप से फ़िट करने के लिए डिज़ाइन किया गया है।\n\n"
            "v4.6.0 में विशेषताएं:\n"
            "- ब्रांड नया उच्च-रिज़ॉल्यूशन एप्लिकेशन आइकन (bttb.ico) और एकीकृत वेबसाइट लोगो (bttb.png)\n"
            "- न्यूनतम खोज स्थिति स्टैक फ्रेम और 16MB Win32 स्टैक सीमा (0xC00000FD ओवरफ़्लो ठीक करना)\n"
            "- अनुरेखण लॉग विच्छेदन से बचने के लिए लॉगिंग बफ़र सीमाओं को 10MB तक बढ़ाना\n"
            "- गैर-अवरोधक वृद्धिशील GUI रेंडरिंग और फ़ाइल क्षमता चेतावनियों को छोड़ना\n"
            "- ऑफ़लाइन JSON अनुक्रमणिका निर्माण और इंटरैक्टिव पार्सर\n"
            "- वैकल्पिक PAR3 पैरिटी फ़ाइल निर्माण और सत्यापन\n"
            "- बिट-परफेक्ट PAR3 कॉपी-आधारित बहाली और मरम्मत\n"
            "- Linux GTK4 पर मानक डार्क थीम विकल्पों सहित थीम समर्थन\n"
            "- स्टार्टअप पर तुरंत सुधारित अनुमानित समय शेष की गणना\n"
            "- नामित कस्टम वॉल्यूम प्रोफ़ाइल और गतिशील ऑटो वॉल्यूम आकार निर्धारण\n"
            "- सेटिंग्स मेमोरी जो अंतिम चयनित वॉल्यूम कॉन्फ़िगरेशन को पुनर्स्थापित करती है\n"
            "- नियम संघर्ष ओवरराइड नियम-आधारित या सिमेंटिक समूहीकरण को जीतने की अनुमति देता है\n"
            "- स्थानांतरण-दर अनुमानित समय शेष और स्थिति गतिविधि स्पिनर\n"
            "- MiniLM एम्बेडिंग पर आधारित एन्ट्रॉपी-अवेयर सिमेंटिक पैकिंग\n"
            "- Windows Explorer राइट-क्लिक मेनू एकीकरण और लंबे पथ समर्थन\n\n"
            "BTTB पूरी तरह से स्थानीयकृत है और जर्मन, डच, फ्रेंच और स्पेनिश में मानक gettext .po टेम्प्लेट के आधार पर पूरे यूजर इंटरफ़ेस का गतिशील रूप से अनुवाद करता है।\n\n"
            "प्रयुक्त पुस्तकालय और आभार:\n"
            "- libpar3 (Yutaka Sawada द्वारा, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3 (BLAKE3 टीम द्वारा, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS (Christopher A. Taylor द्वारा, BSD 3-Clause): https://github.com/catid/leopard\n"
            "- Galois Field लाइब्रेरी (James S. Plank द्वारा): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "लेखक: Sander Raaijmakers, Elwin Oost और Burn to the Brim टीम"
        ),
        "help_guide_text": (
            "1. निर्देशिका विभाजन गहराई\n"
            "वह फ़ोल्डर नेस्टिंग स्तर निर्धारित करता है जिस पर वस्तुएं विभाजित होती हैं:\n"
            " - गहराई 0 (डिफ़ॉल्ट): शीर्ष-स्तरीय फ़ाइलों और फ़ोल्डरों को अलग वस्तुओं के रूप में माना जाता है।\n"
            " - गहराई 1: विभाजन एक स्तर गहरा होता है, जिससे शीर्ष-स्तरीय फ़ोल्डर बरकरार रहते हैं लेकिन उनके तत्काल उप-फ़ोल्डरों को विभाजित किया जाता है।\n\n"
            "2. अधिकतम खोज समय\n"
            "अधिकतम सेकंड जो बैकट्रैकिंग सॉल्वर को चलाने की अनुमति है। यदि यह सीमा पूरी हो जाती है, तो उस समय तक खोजा गया सबसे अच्छा चयन उपयोग किया जाता है।\n\n"
            "3. स्पैनिंग स्लैक\n"
            "एक बार वॉल्यूम पूर्ण अधिकतम क्षमता के इस बाइट्स के भीतर आ जाने पर सॉल्वर को समय से पहले समाप्त करने की अनुमति देता है (जैसे 2048 बाइट्स)।\n\n"
            "4. फ़ाइल/फ़ोल्डर समूहीकरण नियम\n"
            "मेल खाने वाली वस्तुओं को एक ही वॉल्यूम पर एक साथ समूहीकृत रखने के लिए बाध्य करें (जैसे, '*.mp3' या regex '^album_.*' से मिलान)।\n\n"
            "5. एकाधिक स्रोत फ़ोल्डर (+)\n"
            "एकाधिक स्रोत फ़ोल्डरों को निर्दिष्ट करने के लिए '+' पर क्लिक करें। BTTB ऐसे व्यवहार करता है जैसे वे एक ही रूट फ़ोल्डर में हों। नेस्टेड स्रोत पथों को अनदेखा कर दिया जाता है।\n\n"
            "6. सिम्बॉलिक लिंक बनाएं\n"
            "फ़ाइलों को लक्षित फ़ोल्डर में कॉपी/स्थानांतरित करने के बजाय, BTTB आपकी मूल फ़ाइलों की ओर इशारा करते हुए हल्के सिम्बॉलिक लिंक बनाता है।\n\n"
            "7. न्यूरल सिमेंटिक पैकिंग और MiniLM सेटअप गाइड\n"
            "एक सिमेंटिक प्रॉम्प्ट निर्दिष्ट करके, BTTB संदर्भ-जागरूक गहन शिक्षण एम्बेडिंग का उपयोग करके समान सामग्री वाली फ़ाइलों को समूहीकृत करता है।\n"
            "पसंदीदा, उच्च-सटीक MiniLM न्यूरल मॉडल का उपयोग करने के लिए, आपको Python 3 और sentence-transformers स्थापित करना होगा:\n"
            " - चरण 1: सुनिश्चित करें कि Python 3 और pip स्थापित हैं।\n"
            "   (Linux: 'sudo apt install python3 python3-pip python3-venv' चलाएं)\n"
            "   (Windows: https://www.python.org/ से स्थापित करें और 'Add Python to PATH' जांचें)\n"
            " - चरण 2: टर्मिनल या कमांड प्रॉम्प्ट के माध्यम से sentence-transformers स्थापित करें:\n"
            "   विकल्प A (सरलता के लिए अनुशंसित):\n"
            "     pip install sentence-transformers\n"
            "   विकल्प B (आभासी वातावरण अलगाव):\n"
            "     python3 -m venv bttb_env\n"
            "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
            "     pip install sentence-transformers\n"
            " - चरण 3: MiniLM को स्वचालित रूप से लोड करने के लिए Burn to the Brim को पुनरारंभ करें! यदि नहीं मिलता है, तो BTTB आसानी से एक स्थानीयकृत वर्ण TF-IDF प्रोजेक्टर पर वापस आ जाता है।"
        )
    }

    # Now let's define Vulkan (vul - logical, Star Trek themed fictional language)
    # Vulkan uses Star Trek fandom terms or translation patterns.
    # Standard: "System Default (Auto)" -> "Ek'Zhar-kov (Auto)" or similar.
    # Let's write translations that feel logical and Vulkan-themed.
    translations["vul"] = {
        "log_start_restore": "Fa-lok dven-ikov t'voh-lar...\n",
        "log_volume_path": "Path t'voh-lar: ",
        "log_recovery_path": "Path t'zhar-kov: ",
        "log_par3_base": "Name t'PAR3: ",
        "log_copying_repair": "Copying voh-lar and executing repair. Sati'l-pan...\n",
        "log_errors": "Logs/Errors: ",
        "log_success_restore": "\nSpokh: Voh-lar successfully restored and repaired!\n",
        "log_fail_restore": "\nKroykah: Restoration failed.\n",
        "log_start_verify": "Fa-lok verification t'voh-lar...\n",
        "log_success_verify": "\nSpokh: All structures are logical and perfect!\n",
        "log_fail_verify_status": "\nErrors detected (status ",
        "log_damaged_files": "Illogical files found:\n",
        "log_index_fail": "Index verification failed. PAR3 archive is illogical.\n",
        "verify_title": "Verify & Restore Voh-lar...",
        "label_vol_directory": "Directory t'Voh-lar:",
        "browse_btn": "Zvhal...",
        "select_vol_dir_title": "Select Directory t'Voh-lar",
        "label_rec_directory": "Directory t'Recovery:",
        "select_rec_dir_title": "Select Directory t'Recovery",
        "label_par3_base": "Base Name t'PAR3:",
        "log_frame_title_verify": "Verification Log",
        "close_btn": "Dven-kov",
        "verify_only_btn": "Verify Only",
        "restore_repair_btn": "Restore & Repair",
        "iso_title": "Create ISO Image",
        "source_folder": "Source Folder:",
        "target_iso_file": "Target ISO File:",
        "volume_label": "Label t'Voh-lar:",
        "execution_log": "Execution Log",
        "generate_btn": "Fa-lok",
        "complete_list_src_folders": "Complete list of source folders:",
        "add_btn": "Add...",
        "edit_btn": "Edit...",
        "remove_btn": "Remove",
        "cancel_btn": "Kroykah",
        "ok_btn": "Nash-veh",
        "app_title": "Burn to the Brim",
        "app_version": "Version 4.6.0",
        "app_copyright": "Copyright © 2001-2026 Sander Raaijmakers, Elwin Oost and the Vulkan Science Council",
        "select_medium_label": "Select Medium:",
        "auto_size": "Auto Size",
        "custom_size": "Custom Size",
        "capacity_label": "Capacity:",
        "save_cv_label": "Save Custom:",
        "custom_vol_name_placeholder": "Custom Name",
        "save_btn": "Save",
        "cluster_label": "Cluster Size (Bytes):",
        "slack_label": "Slack Bytes:",
        "search_time_label": "Max Search Time (sec):",
        "split_depth_label": "Split Depth:",
        "skip_empty_chk": "Skip Empty Folders",
        "skip_unreadable_chk": "Skip Illogical Files",
        "integrate_ctx_chk": "Integrate with Windows Context Menu",
        "rule_wins_chk": "Logic-based grouping wins:",
        "enable_par3_chk": "Enable PAR3 Protection:",
        "par3_block_size": "PAR3 Block Size (Bytes):",
        "par3_redundancy": "PAR3 Redundancy (%):",
        "language_label": "Vuhlkansu:",
        "grouping_rules_frame": "Logic Grouping Rules",
        "pattern_col": "Pattern",
        "files_col": "Files",
        "folders_col": "Folders",
        "type_col": "Type",
        "rule_pattern_placeholder": "Logic Pattern (*.mp3 or ^[0-9].*\\..*$)",
        "match_files_chk": "Match Files",
        "match_folders_chk": "Match Folders",
        "regex_pattern_chk": "Regex Pattern",
        "add_rule_btn": "Add Rule",
        "remove_selected_btn": "Remove Selected",
        "restart_notice": "Settings saved. Restart Burn to the Brim to apply.",
        "restart_title": "Vuhlkansu Activated",
        "dir_setup_group": "Directories Setup",
        "target_folder": "Target Folder:",
        "semantic_prompt": "Semantic Prompt:",
        "move_files_chk": "Move items to target folder",
        "create_symlinks_chk": "Create logical links (Default)",
        "span_chk": "Span across multiple volumes",
        "trace_chk": "Enable verbose trace logs",
        "progress_frame_title": "Capacity Utilization",
        "filled_label": "Filled: 0.00%",
        "log_frame_title": "Status & Logic Logs",
        "time_left_label": "Time Remaining: --:--",
        "test_btn": "Zvhal (Test)",
        "start_btn": "Fa-lok (Start)",
        "stop_btn": "Kroykah (Stop)",
        "pref_title": "T'Rill",
        "create_iso_btn": "Create ISO...",
        "help_btn": "Kov-es (Help)",
        "about_btn": "Fi' (About)...",
        "import_json_btn": "Import JSON...",
        "verify_restore_btn": "Verify & Restore...",
        "tooltip_import_json": "Import offline JSON indexes.",
        "tooltip_verify_restore": "Verify and restore volumes via PAR3.",
        "tooltip_test": "Simulate packing logical bins.",
        "tooltip_start": "Execute solver to organize files.",
        "tooltip_stop": "Abort current execution.",
        "tooltip_create_iso": "Generate standard ISO filesystem.",
        "tooltip_source": "Semicolon-separated path sources.",
        "tooltip_target": "Destination path for volumes.",
        "tooltip_semantic": "Enter description for MiniLM clustering.",
        "tooltip_move": "Move original files directly.",
        "tooltip_symlink": "Create logical symbolic links.",
        "tooltip_span": "Enable volume spanning.",
        "tooltip_trace": "Log detailed logical steps.",
        "manage_src_folders_title": "Manage Folders",
        "help_guide_title": "BTTB Logic Manual",
        "help_title": "Kov-es - Burn to the Brim",
        "about_title": "Concerning Burn to the Brim",
        "media_size_label": "Medium Size:",
        "save_cv_tooltip": "Save custom capacity parameters",
        "skip_empty_label": "Skip Empty Items:",
        "dark_theme_chk": "Dark Theme:",
        "add_src_dir_title": "Add Source",
        "edit_src_dir_title": "Edit Source",
        "start_tooltip": "Start logical organization",
        "test_tooltip": "Simulate logical organization",
        "stop_tooltip": "Stop execution",
        "rel_path_col": "Path / Category",
        "size_col": "Size",
        "fitted_status_col": "Fit Status",
        "source_tooltip": "Source folders to organize",
        "browse_src_tooltip": "Browse source folders",
        "target_tooltip": "Target volume destination",
        "browse_dest_tooltip": "Browse target destination",
        "select_dest_dir_title": "Select Target",
        "semantic_placeholder": "e.g. keep logical files together",
        "semantic_tooltip": "Cluster similar files via MiniLM AI",
        "move_tooltip": "Move files instead of linking",
        "symlink_tooltip": "Create links instead of copying",
        "span_tooltip": "Span files into multiple volumes",
        "trace_tooltip": "Log logical diagnostics",
        "capacity_recommend_prompt_gtk": "Largest item requires at least %.2f MB capacity.\n\nChoose:",
        "btn_resize": "Resize",
        "btn_skip": "Skip Files",
        "btn_cancel": "Cancel",
        "log_warn_dir_unreadable_1": "Warning: Directory '",
        "log_warn_dir_unreadable_2": "' is unreadable (",
        "log_scan_aborted_unreadable_dir": "Scanning aborted due to unreadable directory.",
        "log_warn_item_unreadable_1": "Warning: Item in '",
        "log_warn_item_unreadable_2": "' is unreadable (",
        "log_scan_aborted_unreadable_item": "Scanning aborted due to unreadable item.",
        "log_warn_failed_iterate_dir_1": "Warning: Iteration failed for '",
        "log_warn_failed_iterate_dir_2": "' (",
        "log_scan_aborted_failed_iterate": "Scanning aborted due to iteration failure.",
        "log_scanning_folder": "Scanning logical folder: ",
        "log_found_items": "Found items: ",
        "log_search_time_limit_exceeded_1": "Time limit exceeded (",
        "log_search_time_limit_exceeded_2": " seconds).",
        "log_new_best_space_utilization": "New optimal space utilization: ",
        "log_selection_within_slack_1": "Selection within slack tolerance (",
        "log_selection_within_slack_2": "%) found. Terminating early.",
        "log_no_items_to_fit": "No items to fit.",
        "log_auto_volume_cap_set_1": "Auto capacity set to: ",
        "log_auto_volume_cap_set_2": " bytes (",
        "log_warn_item_exceeds_cap_1": "Warning: Item '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " bytes) exceeds volume capacity (",
        "log_capacity_adapted_1": "Capacity adapted to ",
        "log_capacity_adapted_2": " bytes. Retrying...",
        "log_capacity_skip_larger": "Larger files skipped. Retrying...",
        "log_solver_aborted_files_exceed_cap": "Solver aborted: items exceed capacity.",
        "log_solver_aborted_files_exceed_cap_adapt": "Solver aborted: items exceed capacity. Adapt capacity or retry.",
        "log_volume_packing_estimation_header": "--- Volume Packing Estimation ---",
        "log_total_dataset_size_1": "Total dataset size: ",
        "log_total_dataset_size_2": " bytes (",
        "log_target_volume_cap_1": "Target volume capacity: ",
        "log_target_volume_cap_2": " bytes (",
        "log_theoretical_min_vols": "Theoretical minimum volumes: ",
        "log_medium_capacity_1": "Medium Capacity: ",
        "log_medium_capacity_2": " sectors (",
        "log_solving_for_vol_header_1": "\n--- SOLVING FOR VOLUME ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "Solving optimal logical packing...",
        "log_finished_solving_1": "Logical search completed in: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "Single volume solution: forcing all items for 100% inclusion...",
        "log_backtracking_failed_fallback_greedy": "Backtracking solver timed out. Falling back to greedy allocation...",
        "log_forcing_progress_exceeds_cap_1": "Forcing item larger than capacity (",
        "log_optimal_selection_covers": "Optimal selection covers: ",
        "log_selected_items_for_volume_1": "Selected items for Volume ",
        "log_selected_items": "Selected items:",
        "log_bytes_suffix": " bytes)",
        "log_organizing_item": "Organizing item: ",
        "log_failed_symlink_retry_copy_1": "Failed to create link for ",
        "log_failed_symlink_retry_copy_2": " (retrying with copy)",
        "log_failed_organize_item_1": "Failed to organize ",
        "log_generating_par3_for_volume_1": "Generating PAR3 protection for Volume ",
        "log_success_par3_for_volume_1": "Created PAR3 protection for Volume ",
        "log_failed_par3_creation": "PAR3 creation failed: ",
        "log_test_simulation_complete": "--- LOGICAL SIMULATION COMPLETE ---",
        "log_no_files_written_on_disk": "No physical changes made on disk.",
        "log_completed_file_organization": "Logical organization completed.",
        "log_created_offline_json_index": "Created offline JSON index: ",
        "log_failed_create_json_index": "Failed to create JSON index: ",
        "log_generating_semantic_embeddings": "Generating semantic embeddings...",
        "log_failed_write_temp_embed_input": "Failed to write temp embedding input: ",
        "log_warn_python_embed_engine_failed": "Warning: Python embedding engine failed.",
        "log_semantic_grouping_fallback": "-> Semantic grouping falls back to string metrics.",
        "log_failed_read_temp_embed_output": "Failed to read temp embedding output: ",
        "log_warn_embedded_output_empty": "Warning: Embedding output is empty.",
        "log_running_semantic_clustering": "Running agglomerative clustering...",
        "log_semantic_clustering_completed": "Logical clustering completed: ",
        "about_comments": (
            "Burn to the Brim (BTTB) is a logical C++20 port of the classic Delphi application designed to optimally fit files and folders onto target storage mediums (CDs, DVDs, Blu-rays, or USBs).\n\n"
            "Logic Features in v4.6.0:\n"
            "- New high-resolution application icon (bttb.ico) and logo (bttb.png)\n"
            "- Minimized search state stack frames & 16MB Win32 stack limit (fixing 0xC00000FD overflows)\n"
            "- Expanded log limits to 10MB to avoid truncation\n"
            "- Non-blocking incremental GUI rendering\n"
            "- Offline JSON Index creation and parser\n"
            "- Optional PAR3 parity file generation\n"
            "- Bit-perfect PAR3 copy-based restoration\n"
            "- Theme support including dark theme options on Linux GTK4\n"
            "- Improved Estimated Time Left calculation\n"
            "- Custom Volume profiles & dynamic Auto Volume sizing\n"
            "- Settings memory restoring the last selected configuration\n"
            "- Conflict overrides allowing rules or semantic grouping to win\n"
            "- Transfer-rate estimated Time Left & spinner\n"
            "- Entropy-Aware Semantic Packing based on MiniLM embeddings\n"
            "- Context Menu integration & long path support\n\n"
            "Authors: Sander Raaijmakers, Elwin Oost and the Vulkan Science Council"
        ),
        "help_guide_text": (
            "1. Split Depth\n"
            "Determines the folder level at which items are split:\n"
            " - Depth 0 (Default): Top-level files and folders are treated as separate.\n"
            " - Depth 1: Splitting occurs one level deeper, keeping top-level folders intact.\n\n"
            "2. Max Search Time\n"
            "The maximum seconds allowed for the backtracking solver. Best selection found is used on timeout.\n\n"
            "3. Spanning Slack\n"
            "Allows early termination when a volume is packed within this number of bytes from the capacity.\n\n"
            "4. Grouping Rules\n"
            "Force matching items to remain grouped together on the same volume.\n\n"
            "5. Multiple Source Folders (+)\n"
            "Click '+' to specify multiple source folders. Nested source paths are ignored.\n\n"
            "6. Create Symbolic Links\n"
            "Creates lightweight logical links pointing back to your original files.\n\n"
            "7. Neural Semantic Packing & MiniLM Setup Guide\n"
            "Groups files with similar content using context-aware deep learning embeddings.\n"
            "To use MiniLM model, Python 3 and sentence-transformers must be installed."
        )
    }

    # Now let's define Elvish (elv - Sindarin/Quenya Tolkien-themed fictional language)
    translations["elv"] = {
        "log_start_restore": "Aiya! Restoring and repairing volume...\n",
        "log_volume_path": "Volume path: ",
        "log_recovery_path": "Recovery path: ",
        "log_par3_base": "PAR3 name: ",
        "log_copying_repair": "Copying volume contents and running repair. Please wait...\n",
        "log_errors": "Logs/Errors: ",
        "log_success_restore": "\nAlassë: Volume successfully copied and repaired!\n",
        "log_fail_restore": "\nFailure: Restoration failed.\n",
        "log_start_verify": "Verifying volume structure...\n",
        "log_success_verify": "\nAlassë: All files verified and are clean/bit-perfect!\n",
        "log_fail_verify_status": "\nIssues detected (status ",
        "log_damaged_files": "Damaged or missing files found:\n",
        "log_index_fail": "Index verification failed. PAR3 archive is corrupted.\n",
        "verify_title": "Verify & Restore Volumes...",
        "label_vol_directory": "Volume Directory:",
        "browse_btn": "Browse...",
        "select_vol_dir_title": "Select Volume Directory",
        "label_rec_directory": "Recovery Directory:",
        "select_rec_dir_title": "Select Recovery Directory",
        "label_par3_base": "PAR3 Base Name:",
        "log_frame_title_verify": "Verification Log",
        "close_btn": "Close",
        "verify_only_btn": "Verify Only",
        "restore_repair_btn": "Restore & Repair",
        "iso_title": "Create ISO Image",
        "source_folder": "Source Folder:",
        "target_iso_file": "Target ISO File:",
        "volume_label": "Volume Label:",
        "execution_log": "Execution Log",
        "generate_btn": "Generate",
        "complete_list_src_folders": "Complete list of source folders:",
        "add_btn": "Add...",
        "edit_btn": "Edit...",
        "remove_btn": "Remove",
        "cancel_btn": "Cancel",
        "ok_btn": "OK",
        "app_title": "Burn to the Brim",
        "app_version": "Version 4.6.0",
        "app_copyright": "Copyright © 2001-2026 Sander Raaijmakers, Elwin Oost and the Elves of Rivendell",
        "select_medium_label": "Select Medium:",
        "auto_size": "Auto Size",
        "custom_size": "Custom Size",
        "capacity_label": "Capacity:",
        "save_cv_label": "Save Custom:",
        "custom_vol_name_placeholder": "Custom Name",
        "save_btn": "Save",
        "cluster_label": "Cluster Size (Bytes):",
        "slack_label": "Slack Bytes:",
        "search_time_label": "Max Search Time (sec):",
        "split_depth_label": "Split Depth:",
        "skip_empty_chk": "Skip Empty Folders",
        "skip_unreadable_chk": "Skip Unreadable Files",
        "integrate_ctx_chk": "Integrate with Windows Context Menu",
        "rule_wins_chk": "Rule-based grouping wins:",
        "enable_par3_chk": "Enable PAR3 Protection:",
        "par3_block_size": "PAR3 Block Size (Bytes):",
        "par3_redundancy": "PAR3 Redundancy (%):",
        "language_label": "Eldarin:",
        "grouping_rules_frame": "File / Folder Grouping Rules",
        "pattern_col": "Pattern",
        "files_col": "Files",
        "folders_col": "Folders",
        "type_col": "Type",
        "rule_pattern_placeholder": "Rule Pattern (*.mp3 or ^[0-9].*\\..*$)",
        "match_files_chk": "Match Files",
        "match_folders_chk": "Match Folders",
        "regex_pattern_chk": "Regex Pattern",
        "add_rule_btn": "Add Rule",
        "remove_selected_btn": "Remove Selected",
        "restart_notice": "Settings saved. Restart Burn to the Brim to apply.",
        "restart_title": "Eldarin Activated",
        "dir_setup_group": "Directories Setup",
        "target_folder": "Target Folder:",
        "semantic_prompt": "Semantic Prompt:",
        "move_files_chk": "Move items to target folder",
        "create_symlinks_chk": "Create symbolic links (Default)",
        "span_chk": "Span across multiple volumes",
        "trace_chk": "Enable detailed trace logs",
        "progress_frame_title": "Fitted Medium Capacity",
        "filled_label": "Filled: 0.00%",
        "log_frame_title": "Status and Solver Logs",
        "time_left_label": "Time Left: --:--",
        "test_btn": "Tyasta (Test)",
        "start_btn": "Hesta (Start)",
        "stop_btn": "Pusta (Stop)",
        "pref_title": "Indemi",
        "create_iso_btn": "Create ISO...",
        "help_btn": "Tulu (Help)",
        "about_btn": "Giriath (About)...",
        "import_json_btn": "Import JSON...",
        "verify_restore_btn": "Verify & Restore...",
        "tooltip_import_json": "Import and parse offline JSON indices.",
        "tooltip_verify_restore": "Verify solved volumes or restore damaged files via PAR3.",
        "tooltip_test": "Simulate packing without writing to disk.",
        "tooltip_start": "Run solver and organize files.",
        "tooltip_stop": "Abort current running packing.",
        "tooltip_create_iso": "Create ISO filesystem images.",
        "tooltip_source": "Semicolon-separated folder paths.",
        "tooltip_target": "Destination path for volumes.",
        "tooltip_semantic": "Enter a prompt for MiniLM analysis.",
        "tooltip_move": "Move original files directly.",
        "tooltip_symlink": "Create filesystem symbolic links.",
        "tooltip_span": "Enable volume spanning.",
        "tooltip_trace": "Log detailed diagnostics.",
        "manage_src_folders_title": "Manage Folders",
        "help_guide_title": "BTTB Help Manual",
        "help_title": "Tulu - Burn to the Brim",
        "about_title": "Concerning Burn to the Brim",
        "media_size_label": "Medium Size:",
        "save_cv_tooltip": "Save current capacity parameters",
        "skip_empty_label": "Skip Empty Items:",
        "dark_theme_chk": "Dark Theme:",
        "add_src_dir_title": "Add Source",
        "edit_src_dir_title": "Edit Source",
        "start_tooltip": "Start organizing files",
        "test_tooltip": "Simulate organizing files",
        "stop_tooltip": "Stop current solver",
        "rel_path_col": "Path / Category",
        "size_col": "Size",
        "fitted_status_col": "Fit Status",
        "source_tooltip": "Source folders to organize",
        "browse_src_tooltip": "Browse source folders",
        "target_tooltip": "Target volume destination",
        "browse_dest_tooltip": "Browse target destination",
        "select_dest_dir_title": "Select Target",
        "semantic_placeholder": "e.g. keep similar content together",
        "semantic_tooltip": "Cluster similar files via MiniLM AI",
        "move_tooltip": "Move files instead of linking",
        "symlink_tooltip": "Create links instead of copying",
        "span_tooltip": "Span files into multiple volumes",
        "trace_tooltip": "Log diagnostics in logs",
        "capacity_recommend_prompt_gtk": "Largest item requires at least %.2f MB capacity.\n\nChoose:",
        "btn_resize": "Resize",
        "btn_skip": "Skip Files",
        "btn_cancel": "Cancel",
        "log_warn_dir_unreadable_1": "Warning: Directory '",
        "log_warn_dir_unreadable_2": "' is unreadable (",
        "log_scan_aborted_unreadable_dir": "Scanning aborted due to unreadable directory.",
        "log_warn_item_unreadable_1": "Warning: Item in '",
        "log_warn_item_unreadable_2": "' is unreadable (",
        "log_scan_aborted_unreadable_item": "Scanning aborted due to unreadable item.",
        "log_warn_failed_iterate_dir_1": "Warning: Iteration failed for '",
        "log_warn_failed_iterate_dir_2": "' (",
        "log_scan_aborted_failed_iterate": "Scanning aborted due to iteration failure.",
        "log_scanning_folder": "Scanning folder: ",
        "log_found_items": "Found items: ",
        "log_search_time_limit_exceeded_1": "Search time limit exceeded (",
        "log_search_time_limit_exceeded_2": " seconds).",
        "log_new_best_space_utilization": "New optimal space utilization: ",
        "log_selection_within_slack_1": "Selection within slack tolerance (",
        "log_selection_within_slack_2": "%) found. Terminating early.",
        "log_no_items_to_fit": "No items to fit.",
        "log_auto_volume_cap_set_1": "Auto capacity set to: ",
        "log_auto_volume_cap_set_2": " bytes (",
        "log_warn_item_exceeds_cap_1": "Warning: Item '",
        "log_warn_item_exceeds_cap_2": "' (",
        "log_warn_item_exceeds_cap_3": " bytes) exceeds volume capacity (",
        "log_capacity_adapted_1": "Capacity adapted to ",
        "log_capacity_adapted_2": " bytes. Retrying solver...",
        "log_capacity_skip_larger": "Larger files skipped. Retrying solver...",
        "log_solver_aborted_files_exceed_cap": "Solver aborted: items exceed capacity.",
        "log_solver_aborted_files_exceed_cap_adapt": "Solver aborted: items exceed capacity. Adapt capacity or retry.",
        "log_volume_packing_estimation_header": "--- Volume Packing Estimation ---",
        "log_total_dataset_size_1": "Total dataset size: ",
        "log_total_dataset_size_2": " bytes (",
        "log_target_volume_cap_1": "Target volume capacity: ",
        "log_target_volume_cap_2": " bytes (",
        "log_theoretical_min_vols": "Theoretical minimum volumes: ",
        "log_medium_capacity_1": "Medium Capacity: ",
        "log_medium_capacity_2": " sectors (",
        "log_solving_for_vol_header_1": "\n--- SOLVING FOR VOLUME ",
        "log_solving_for_vol_header_2": " ---",
        "log_solving_optimal_bin": "Solving optimal selection...",
        "log_finished_solving_1": "Finished solving. Elapsed time: ",
        "log_finished_solving_2": " ms",
        "log_single_vol_sol_forcing": "Single volume solution: forcing all items...",
        "log_backtracking_failed_fallback_greedy": "Backtracking solver timed out. Falling back to greedy allocation...",
        "log_forcing_progress_exceeds_cap_1": "Packing item larger than capacity (",
        "log_optimal_selection_covers": "Optimal selection covers: ",
        "log_selected_items_for_volume_1": "Selected items for Volume ",
        "log_selected_items": "Selected items:",
        "log_bytes_suffix": " bytes)",
        "log_organizing_item": "Organizing item: ",
        "log_failed_symlink_retry_copy_1": "Failed to create link for ",
        "log_failed_symlink_retry_copy_2": " (retrying with copy)",
        "log_failed_organize_item_1": "Failed to organize ",
        "log_generating_par3_for_volume_1": "Generating PAR3 protection for Volume ",
        "log_success_par3_for_volume_1": "Created PAR3 protection for Volume ",
        "log_failed_par3_creation": "PAR3 creation failed: ",
        "log_test_simulation_complete": "--- SIMULATION COMPLETE ---",
        "log_no_files_written_on_disk": "No changes made on disk.",
        "log_completed_file_organization": "File organization completed.",
        "log_created_offline_json_index": "Created offline JSON index: ",
        "log_failed_create_json_index": "Failed to create JSON index: ",
        "log_generating_semantic_embeddings": "Generating semantic embeddings...",
        "log_failed_write_temp_embed_input": "Failed to write temp input: ",
        "log_warn_python_embed_engine_failed": "Warning: Python embedding engine failed.",
        "log_semantic_grouping_fallback": "-> Semantic grouping falls back to string metrics.",
        "log_failed_read_temp_embed_output": "Failed to read temp embedding output: ",
        "log_warn_embedded_output_empty": "Warning: Embedding output is empty.",
        "log_running_semantic_clustering": "Running agglomerative clustering...",
        "log_semantic_clustering_completed": "Clustering completed: ",
        "about_comments": (
            "Burn to the Brim (BTTB) is a modern C++20 port of the classic Delphi application designed to optimally fit files and folders onto target storage mediums (CDs, DVDs, Blu-rays, or USBs).\n\n"
            "Features in v4.6.0:\n"
            "- New application icon (bttb.ico) and website logo (bttb.png)\n"
            "- Minimized search state stack frames & 16MB Win32 stack limit (fixing 0xC00000FD overflows)\n"
            "- Expanded logging limits to 10MB to avoid truncation\n"
            "- Non-blocking incremental GUI rendering\n"
            "- Offline JSON Index creation and parser\n"
            "- Optional PAR3 parity file generation and verification\n"
            "- Bit-perfect PAR3 copy-based restoration and repair\n"
            "- Theme support including standard dark theme options on Linux GTK4\n"
            "- Improved Estimated Time Left calculation immediately at startup\n"
            "- Named Custom Volume profiles & dynamic Auto Volume sizing\n"
            "- Settings memory restoring the last selected volume configuration\n"
            "- Rule conflict overrides allowing rule-based or semantic grouping to win\n"
            "- Transfer-rate estimated Time Left & status activity spinner\n"
            "- Entropy-Aware Semantic Packing based on MiniLM embeddings\n"
            "- Explorer Context Menu integration & long path support\n\n"
            "Authors: Sander Raaijmakers, Elwin Oost and the Elves of Rivendell"
        ),
        "help_guide_text": (
            "1. Split Depth\n"
            "Determines the folder nesting level at which items are split:\n"
            " - Depth 0 (Default): Top-level files and folders are treated as separate items.\n"
            " - Depth 1: Splitting occurs one level deeper, keeping top-level folders intact.\n\n"
            "2. Max Search Time\n"
            "The maximum seconds the backtracking solver is allowed to run.\n\n"
            "3. Spanning Slack\n"
            "Allows early solver termination once a volume is packed within this number of bytes.\n\n"
            "4. File/Folder Grouping Rules\n"
            "Force matching items to remain grouped together on the same volume.\n\n"
            "5. Multiple Source Folders (+)\n"
            "Specify multiple source folders. Nested source paths are ignored.\n\n"
            "6. Create Symbolic Links\n"
            "Creates lightweight symbolic links pointing back to original files.\n\n"
            "7. Neural Semantic Packing & MiniLM Setup Guide\n"
            "Groups files with similar content using context-aware deep learning embeddings."
        )
    }

    # Write each translation mapping to its own .po file
    for lang, translations_map in translations.items():
        po_lines = []
        # Add keys in alphabetical order for consistency
        for key in sorted(keys_dict.keys()):
            val = translations_map.get(key, keys_dict[key])
            
            # Format value as double quoted string, escaping newlines, backslashes, quotes
            # Wait, BttbLocale parses msgid "..." and msgstr "..."
            # Escaping the value for PO format
            escaped_val = val.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n').replace('\r', '\\r').replace('\t', '\\t')
            
            # Key also needs escaping if it contains special characters, but our keys are simple alphanumeric strings
            escaped_key = key.replace('\\', '\\\\').replace('"', '\\"')
            
            po_lines.append(f'msgid "{escaped_key}"')
            po_lines.append(f'msgstr "{escaped_val}"')
            po_lines.append('')
            
        po_content = '\n'.join(po_lines)
        po_path = f"lang/{lang}.po"
        with open(po_path, "w", encoding="utf-8") as po_file:
            po_file.write(po_content)
        print(f"Generated {po_path}")

if __name__ == '__main__':
    main()
