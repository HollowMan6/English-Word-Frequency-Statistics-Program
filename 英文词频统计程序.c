#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

FILE * fdest;

typedef struct 
{
	char word[30];
	int count;
} Item;

typedef struct node {
	Item item;
	struct node * left;
	struct node * right;
} Node;

typedef Node * Tree;

Tree * InitializeTree(Tree * ptree) {
	*ptree = NULL;
	return ptree;
}

bool TreeIsEmpty(const Tree * ptree) {
	return (*ptree == NULL);
}

bool TreeIsFull(const Tree * ptree) {
	Node * pnode = (Node *) malloc(sizeof(Node));
	if (pnode == NULL) {
		return true;
	}
	else {
		free(pnode);
		return false;
	}
}

bool AddWord(const char * word, Tree * ptree) {
	if (TreeIsFull(ptree)) {
		return false;
	}

	int cmp;
	while (*ptree != NULL) {
		int cmp = strcmp(word, (*ptree)->item.word);
		if (cmp == 0) {
			(*ptree)->item.count++;
			return true;
		}
		else if (cmp < 0) {
			ptree = &((*ptree)->left);
		}
		else {
			ptree = &((*ptree)->right);
		}
	}

	Item item;
	strncpy(item.word, word, 30);
	item.count = 1;

	Node * pnode = (Node *) malloc(sizeof(Node));
	if (pnode == NULL) {
		fprintf(stderr, "内存不足！\n");
		return false;
	}

	pnode->item = item;
	pnode->left = NULL;
	pnode->right = NULL;

	*ptree = pnode;
	return true;
}

bool DeleteWord(const char * word, Tree * ptree) {
	while (*ptree != NULL) {
		int cmp = strcmp(word, (*ptree)->item.word);
		if (cmp == 0) {
			Node * right = (*ptree)->right;
			Node * tmp = (*ptree);
			*ptree = (*ptree)->left;
			while ((*ptree)->right != NULL) {
				ptree = &((*ptree)->right);
			}
			(*ptree)->right = right;

			free(tmp);
			return true;
		}
		else if (cmp < 0) {
			ptree = &((*ptree)->left);
		}
		else {
			ptree = &((*ptree)->right);
		}
	}
	return false;

}

bool InTree(const char * word, const Tree * ptree) {
	Node * pnode = *ptree;
	while (pnode != NULL) {
		int cmp = strcmp(word, pnode->item.word);
		if (cmp == 0) {
			return true;
		}
		else if (cmp < 0) {
			pnode = pnode->left;
		}
		else {
			pnode = pnode->right;
		}
	}
	return false;
}

void ApplyToNode(const char * word, const Tree * ptree, void (*fn)(Item)) {
	Node * pnode = *ptree;
	while (pnode != NULL) {
		int cmp = strcmp(word, pnode->item.word);
		if (cmp == 0) {
			(*fn)(pnode->item);
			return;
		}
		else if (cmp < 0) {
			pnode = pnode->left;
		}
		else {
			pnode = pnode->right;
		}
	}
	return;
}

void ApplyToAll(const Tree * ptree, void (*fn)(Item)) {
	Node * pnode = *ptree;
	if (pnode == NULL) {
		return;
	}
	ApplyToAll(&(pnode->left), fn);
	(*fn)(pnode->item);
	ApplyToAll(&(pnode->right), fn);
}

void DeleteAll(Tree * ptree) {
	Node * pnode = *ptree;
	DeleteAll(&(pnode->left));
	DeleteAll(&(pnode->right));
	free(pnode);
}

void printMenu(void) {
	puts("/***************************/");
	puts("           选项:");
	puts("---------------------------");
	puts("a.列出所有的单词及出现的次数");
	puts("b.寻找一个单词在文中出现的次数");
	puts("q.退出");
	puts("---------------------------");
	puts("/***************************/");
	printf("请选择: ");
}

int getOption(void) {
	printMenu();
	int opt;
	while ((opt = getchar()) != 'a' && opt != 'b' && opt != 'q') {
		printf("无效选择，请重试: ");
		while (getchar() != '\n')
			continue;
	}
	while (getchar() != '\n')
		continue;
	return opt;
}

void printWordCount(Item item) {
	fprintf(fdest,"%s  出现:%d次\n", item.word, item.count);
}

char * toLowercase(char * string) {
	char * retval = string;
	for (; *string != '\0'; string++) {
		*string = tolower(*string);
	}
	return retval;
}

char * get(char * string, int n) {

	char * retval = fgets(string, n, stdin);

	for (; *string != '\0'; string++) {
		if (*string == '\n') {
			*string = '\0';
			break;
		}
	}
	return retval;
}

int main(int argc, char *argv[]) 
{
	
	FILE * fp;
	char source[128];
	char dest[128];
	char way[2];
	puts("    英文词频统计程序           ***Powered By Hollow Man***");
	puts("**********************************************************");
	printf("输入需要统计词频的文件: ");
	get(source, 128);
	puts("");
	printf("输入写入统计数据文件: ");
	get(dest, 128);
	puts("");
	printf("输入统计数据写入方式(a代表文件末尾追加,w代表覆盖写入):");
	way[1]='\0';
	while ((way[0]=getchar()) != 'a' && way[0] != 'w') {
		puts("");
		printf("%s",way);
		printf("是无效选择，请重试: ");
		while (getchar() != '\n')
			continue;
	}
	getchar();
	if ((fp = fopen(source, "r")) == NULL)
	{
		puts("");
		fprintf(stderr, "无法打开读取的文件 %s.\n", source);
		exit(EXIT_FAILURE);
	}
	if ((fdest = fopen(dest, way)) == NULL)
	{
		puts("");
		fprintf(stderr, "无法打开写入的文件 %s.\n", dest);
		exit(EXIT_FAILURE);
	}

	char tmp[30];
	Tree words;
	Tree * ptree = InitializeTree(&words);
	while (fscanf(fp, "%s", tmp) == 1) {
		toLowercase(tmp);
		for(int i=0;i<30;i++)
			if(tmp[i]>'z'||tmp[i]<'a')
			{
				tmp[i]='\0';
				break;
			}
		if (!AddWord(tmp, ptree)) {
			puts("");
			fprintf(stderr, "未能将 %s 添加到查找范围中.\n",tmp);
			exit(1);
		}
	}

	int ch;
	char word[30];
	while ((ch = getOption()) != 'q') {
		if (ch == 'a') {
			puts("");
			puts("正在处理中，请稍后......");
			ApplyToAll(ptree, printWordCount);
			puts("");
			puts("成功执行操作！请关闭程序后查看！");
		} else if (ch == 'b') {
			puts("");
			printf("输入要搜索的单词: ");
			get(word, 30);
			puts("");
			puts("正在处理中，请稍后......");
			ApplyToNode(word, ptree, printWordCount);
			puts("");
			puts("成功执行操作！请关闭程序后查看！");
		}
		puts("");
	}
	fclose(fp);
	fclose(fdest);
}