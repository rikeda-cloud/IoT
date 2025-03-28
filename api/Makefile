DCCOMPOSE = docker compose

.DEFAULT_GOAL := up

# Docker コンテナの実行
up:
	${DCCOMPOSE} up -d --build

down:
	${DCCOMPOSE} down --rmi all --volumes --remove-orphans
