## How It Works

`Filtered Array` packs all entities into a single contiguous array (`data`) and uses an array of indices (`cursors`) to keep track of state boundaries without allocating anything more memory.

### Memory Layout

For `NUM_STATES = 3` and `CAPACITY = 8`:

```text
cursors[] = [  0,        2,              5,               6,        6  ]
               │         │               │                │         │
               ▼         ▼               ▼                ▼         ▼
Index:         0    1    2    3    4     5                6    7
            ┌────┬────┬────┬────┬────┬───────┬──────────────────────┐
data[]:     │ A1 │ A2 │ B1 │ B2 │ B3 │  C1   │  Unused Capacity...  │
            └────┴────┴────┴────┴────┴───────┴──────────────────────┘
            └─────────┴──────────────┴───────┴──────────────────────┘
              State 0     State 1     State 2   State 3 (empty)
             [0..1]      [2..4]       [5..5]     [6..6]
```

## Example on how to use it

```C
#include <stdio.h>
#include <stdint.h>
#include "arrayFilterUtility/arrayFilterUtility.h"

typedef enum {
    SLOT_HELMET,
    SLOT_CHEST,
    SLOT_GLOVES,
    SLOT_BELT,
    SLOT_BOOTS,
    SLOT_COUNT
} EquipmentSlot;

typedef struct {
    uint32_t id;
    uint16_t defense;
    uint16_t durability;
    char name[32];
} ArmorItem;

#define MAX_ARMOR_CAPACITY 128
DEFINE_FILTERED_ARRAY(ArmorItem, ArmorInventory, uint16_t, SLOT_COUNT, MAX_ARMOR_CAPACITY)

int main(void) {
    ArmorInventory inv;
    ArmorInventory_init(&inv); // or just ArmorInventory inv = {0}; on the stack

    ArmorInventory_push(&inv, SLOT_HELMET, (ArmorItem){1, 15, 100, "Iron Helmet"});
    ArmorInventory_push(&inv, SLOT_CHEST,  (ArmorItem){2, 45, 150, "Steel Cuirass"});
    ArmorInventory_push(&inv, SLOT_CHEST,  (ArmorItem){3, 60, 200, "Chainmail"});
    ArmorInventory_push(&inv, SLOT_GLOVES, (ArmorItem){4, 10,  80, "Leather Gloves"});
    ArmorInventory_push(&inv, SLOT_BELT,   (ArmorItem){5,  5,  50, "Cloth Belt"});
    ArmorInventory_push(&inv, SLOT_BOOTS,  (ArmorItem){6, 12,  90, "Boots of Speed"});

    uint16_t chest_count = 0;
    ArmorItem *chests = ArmorInventory_get_range(&inv, SLOT_CHEST, &chest_count);

    printf("=== CHESTS TAB (%u) ===\n", chest_count);
    for (uint16_t i = 0; i < chest_count; i++) {
        printf("- %s | Defense: %u | Durability: %u%%\n", 
               chests[i].name, chests[i].defense, chests[i].durability);
    }

    printf("\n=== INVENTORY SUMMARY ===\n");
    const char *slot_names[] = {"Helmets", "Chests", "Gloves", "Belts", "Boots"};
    
    for (size_t s = 0; s < SLOT_COUNT; s++) {
        printf("- %s: %u item(s)\n", slot_names[s], ArmorInventory_count(&inv, s));
    }

    printf("Total items in inventory: %u / 128\n", ArmorInventory_total_count(&inv));

    return 0;
}
```

output result:
```
=== CHESTS TAB (2) ===
- Chainmail | Defense: 60 | Durability: 200%
- Steel Cuirass | Defense: 45 | Durability: 150%

=== INVENTORY SUMMARY ===
- Helmets: 1 item(s)
- Chests: 2 item(s)
- Gloves: 1 item(s)
- Belts: 1 item(s)
- Boots: 1 item(s)
Total items in inventory: 6 / 128
```
