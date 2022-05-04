#include "a1.h"

/**
	Add your functions to this file.
	Make sure to review a1.h.
	Do NOT include a main() function in this file
	when you submit.
*/

Restaurant *initialize_restaurant(char *name)
{
	// initialize restaurant
	struct Restaurant *new_restaurant = (Restaurant *)malloc(sizeof(struct Restaurant));
	// set name field
	new_restaurant->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(new_restaurant->name, name);
	// initialize menu
	new_restaurant->menu = load_menu(MENU_FNAME);
	// initalize number of completed orders
	new_restaurant->num_completed_orders = 0;
	// initalize number of pending orders
	new_restaurant->num_pending_orders = 0;
	// initalize pending orders and set head and tail as NULL
	new_restaurant->pending_orders = (Queue *)malloc(sizeof(struct Queue));
	new_restaurant->pending_orders->head = NULL;
	new_restaurant->pending_orders->tail = NULL;

	return new_restaurant;
}

Menu *load_menu(char *fname)
{
	// dynamically allocate memory for the menu
	struct Menu *new_menu = (Menu *)malloc(sizeof(Menu));

	// read in file
	FILE *fp = fopen(fname, "r");

	// max amount of characters in a line
	int line_length = ITEM_CODE_LENGTH + MAX_ITEM_NAME_LENGTH + 10;

	// temp variable to store each line when reading file
	char buffer[line_length + 1];

	// number of items in menu
	new_menu->num_items = 0;

	// get the number of items in the menu
	while (fgets(buffer, line_length + 1, fp))
	{
		new_menu->num_items++;
	}
	fclose(fp);

	// reset fp to the file
	fp = fopen(fname, "r");

	// temp string to hold data from file
	char file_data[new_menu->num_items * line_length];
	file_data[0] = '\0';

	/*
	int count = 1;
	*/
	// add data from file to string
	while (fgets(buffer, line_length + 1, fp))
	{
		while (*buffer == ' ')
		{
			strcpy(buffer, buffer + 1);
		}
		/*
		int end_index = strlen(buffer) - 1;
		while (*(buffer + end_index) == ' ' || *(buffer + end_index) == '\n' || *(buffer + end_index) == '\0')
		{
			buffer[end_index] = '\0';
			end_index--;
		}
		buffer[strlen(buffer)] = '\0';
		if (count != new_menu->num_items)
		{
			buffer[strlen(buffer)] = '\n';
			buffer[strlen(buffer) + 1] = '\0';
		}
		count++;
		*/
		strcat(file_data, buffer);
	}

	// close file
	fclose(fp);

	// dynamically allocate memory for item codes, item names, and item cost per unit
	new_menu->item_codes = (char **)malloc(sizeof(char *) * new_menu->num_items);
	for (int i = 0; i < new_menu->num_items; i++)
	{
		new_menu->item_codes[i] = (char *)malloc(sizeof(char) * ITEM_CODE_LENGTH);
	}
	new_menu->item_names = (char **)malloc(sizeof(char *) * new_menu->num_items);
	for (int i = 0; i < new_menu->num_items; i++)
	{
		new_menu->item_names[i] = (char *)malloc(sizeof(char) * MAX_ITEM_NAME_LENGTH);
	}
	new_menu->item_cost_per_unit = (double *)malloc(sizeof(double) * new_menu->num_items);

	int line_number = 0;
	int line_section = 0;
	char *token = strtok(file_data, MENU_DELIM);

	while (token != NULL)
	{
		if (line_section == 0)
		{
			strcpy((new_menu->item_codes)[line_number], token);
			token = strtok(NULL, MENU_DELIM);
		}
		else if (line_section == 1)
		{
			strcpy(new_menu->item_names[line_number], token);
			token = strtok(NULL, "\n");
		}
		else
		{
			new_menu->item_cost_per_unit[line_number] = atof(token + 1);
			token = strtok(NULL, MENU_DELIM);
		}
		line_section++;
		if (line_section == 3)
		{
			line_section = 0;
			line_number++;
		}
	}

	return new_menu;
}

Order *build_order(char *items, char *quantities)
{
	struct Order *new_order = (Order *)malloc(sizeof(Order));
	new_order->num_items = strlen(items) / (ITEM_CODE_LENGTH - 1);
	new_order->item_codes = (char **)malloc(sizeof(char *) * new_order->num_items);
	new_order->item_quantities = (int *)malloc(sizeof(int) * new_order->num_items);

	for (int i = 0; i < new_order->num_items; i++)
	{
		new_order->item_codes[i] = (char *)malloc(sizeof(char) * (ITEM_CODE_LENGTH));
	}

	int start_index = 0;

	while (start_index != new_order->num_items)
	{
		new_order->item_codes[start_index][0] = items[start_index * 2];
		new_order->item_codes[start_index][1] = items[start_index * 2 + 1];
		new_order->item_codes[start_index][2] = '\0';
		start_index += 1;
	}

	// can't modify quantities because it is a pointer so make a copy
	char temp_quantities[strlen(quantities)];
	strcpy(temp_quantities, quantities);

	char *token = strtok(temp_quantities, MENU_DELIM);
	int index = 0;
	while (token != NULL)
	{
		new_order->item_quantities[index] = atoi(token);
		index++;
		token = strtok(NULL, MENU_DELIM);
	}

	return new_order;
}

void enqueue_order(Order *order, Restaurant *restaurant)
{
	// initalize new QueueNode
	struct QueueNode *new_node = (struct QueueNode *)malloc(sizeof(struct QueueNode));
	new_node->order = order;
	new_node->next = NULL;
	restaurant->num_pending_orders++;

	// check if Queue is empty
	if (restaurant->pending_orders->tail == NULL)
	{
		restaurant->pending_orders->head = restaurant->pending_orders->tail = new_node;
		return;
	}

	// rewire nodes
	restaurant->pending_orders->tail->next = new_node;
	restaurant->pending_orders->tail = new_node;
}

Order *dequeue_order(Restaurant *restaurant)
{
	struct QueueNode *temp = restaurant->pending_orders->head;
	struct Order *removed_order = restaurant->pending_orders->head->order;

	restaurant->pending_orders->head = restaurant->pending_orders->head->next;
	if (restaurant->pending_orders->head == NULL)
	{ // queue empty
		restaurant->pending_orders->head = NULL;
		restaurant->pending_orders->tail = NULL;
	}
	restaurant->num_pending_orders--;
	restaurant->num_completed_orders++;
	free(temp);
	return removed_order;
}

double get_item_cost(char *item_code, Menu *menu)
{
	for (int i = 0; i < menu->num_items; i++)
	{
		if (strcmp(menu->item_codes[i], item_code) == 0)
		{
			return menu->item_cost_per_unit[i];
		}
	}
}

double get_order_subtotal(Order *order, Menu *menu)
{
	double temp_price;
	double subtotal = 0;
	for (int i = 0; i < order->num_items; i++)
	{
		temp_price = get_item_cost(order->item_codes[i], menu) * order->item_quantities[i];
		subtotal += temp_price;
	}
	return subtotal;
}

double get_order_total(Order *order, Menu *menu)
{
	double subtotal = get_order_subtotal(order, menu);
	return subtotal * TAX_RATE / 100 + subtotal;
}

int get_num_completed_orders(Restaurant *restaurant)
{
	return restaurant->num_completed_orders;
}

int get_num_pending_orders(Restaurant *restaurant)
{
	return restaurant->num_pending_orders;
}

void clear_order(Order **order)
{
	for (int i = 0; i < (*order)->num_items; i++)
	{
		free((*order)->item_codes[i]);
	}
	free((*order)->item_codes);
	free((*order)->item_quantities);
	free(*order);
	*order = NULL;
}

void clear_menu(Menu **menu)
{

	for (int i = 0; i < (*menu)->num_items; i++)
	{
		free((*menu)->item_codes[i]);
		free((*menu)->item_names[i]);
	}
	free((*menu)->item_codes);
	free((*menu)->item_names);
	free((*menu)->item_cost_per_unit);
	free(*menu);
	*menu = NULL;
}

void close_restaurant(Restaurant **restaurant)
{
	while ((*restaurant)->pending_orders->head != NULL)
	{
		clear_order(&(*restaurant)->pending_orders->head->order);
		struct QueueNode *temp = (*restaurant)->pending_orders->head;
		(*restaurant)->pending_orders->head = (*restaurant)->pending_orders->head->next;
		free(temp);
	}
	clear_menu(&(*restaurant)->menu);
	free((*restaurant)->name);
	free((*restaurant)->pending_orders);
	free(*restaurant);
	*restaurant = NULL;
}

void print_menu(Menu *menu)
{
	fprintf(stdout, "--- Menu ---\n");
	for (int i = 0; i < menu->num_items; i++)
	{
		fprintf(stdout, "(%s) %s: %.2f\n",
				menu->item_codes[i],
				menu->item_names[i],
				menu->item_cost_per_unit[i]);
	}
}

void print_order(Order *order)
{
	for (int i = 0; i < order->num_items; i++)
	{
		fprintf(
			stdout,
			"%d x (%s)\n",
			order->item_quantities[i],
			order->item_codes[i]);
	}
}

void print_receipt(Order *order, Menu *menu)
{
	for (int i = 0; i < order->num_items; i++)
	{
		double item_cost = get_item_cost(order->item_codes[i], menu);
		fprintf(
			stdout,
			"%d x (%s)\n @$%.2f ea \t %.2f\n",
			order->item_quantities[i],
			order->item_codes[i],
			item_cost,
			item_cost * order->item_quantities[i]);
	}
	double order_subtotal = get_order_subtotal(order, menu);
	double order_total = get_order_total(order, menu);

	fprintf(stdout, "Subtotal: \t %.2f\n", order_subtotal);
	fprintf(stdout, "               -------\n");
	fprintf(stdout, "Tax %d%%: \t$%.2f\n", TAX_RATE, order_total);
	fprintf(stdout, "              ========\n");
}
