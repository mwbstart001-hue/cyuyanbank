FROM debian:bullseye-slim

# 安装编译环境和 SQLite
RUN apt-get update && apt-get install -y \
    build-essential \
    libsqlite3-dev \
    sqlite3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 复制源代码
COPY . .

# 编译程序
RUN make clean && make

# 运行程序
CMD ["./bank_system"]
