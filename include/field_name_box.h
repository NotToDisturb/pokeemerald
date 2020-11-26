#ifndef GUARD_FIELD_NAME_BOX_H
#define GUARD_FIELD_NAME_BOX_H

void DrawNameFrame(u8 windowId, bool8 copyToVram);
void LoadNameboxWindow(const struct WindowTemplate *windowTemplate);
void LoadNameboxSprite(void);
void AddTextPrinterForName(void);
bool8 IsNameboxDisplayed(void);
void ClearNamebox(void);
void ShowFieldName(const u8 *str);

#endif // GUARD_FIELD_NAME_BOX_H
