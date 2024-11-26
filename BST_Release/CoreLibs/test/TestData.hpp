/* Description: Predefined constants and functions used for testing.
 * Language:C++
 * Author:***
 */

#ifndef LIB_TESTDATA
#define LIB_TESTDATA

extern unsigned char TestData_Str_Pattern(UBINT Index){
	//returns the character of a pattern at [Index].
	//pattern: LINExxxxxxx:ABCD....\n (64 characters for each line)

	const unsigned char TmpStr[5] = "LINE";
	UBINT Col = Index & 0x3F;
	if (Col<4)return TmpStr[Col];
	else if (4 <= Col && 11>Col){
		Col = (Index >> (46 - Col * 4)) & 0xF;
		if (Col >= 10)return 'A' + (unsigned char)(Col - 10); else return '0' + (unsigned char)Col;
	}
	else if (11 == Col)return ':';
	else if (0x3F == Col)return '\n';
	else return (unsigned char)(28 + Col);
	return 0;
}

const unsigned char TestData_Str_UTF8[0x4A] = {
	//This string is used for validate the correctness of Unicode coders/decoders.
	0xEF, 0xBB, 0xBF, 0xED, 0xA0, 0xB0, 0xF0, 0x9D,
	0x84, 0x90, 0xC2, 0xA2, 0xAC, 0x82, 0xF0, 0xA4,
	0xAD, 0xFE, 0xE8, 0xBF, 0x99, 0xE6, 0x98, 0xAF,
	0xE4, 0xB8, 0x80, 0xE4, 0xBB, 0xBD, 0xE7, 0x94,
	0xA8, 0xE4, 0xBA, 0x8E, 0xE6, 0xB5, 0x8B, 0xE8,
	0xAF, 0x95, 0x55, 0x6E, 0x69, 0x63, 0x6F, 0x64,
	0x65, 0xE7, 0xBC, 0x96, 0x2F, 0xE8, 0xA7, 0xA3,
	0xE7, 0xA0, 0x81, 0xE5, 0x99, 0xA8, 0xE7, 0x9A,
	0x84, 0xE6, 0x96, 0x87, 0xE4, 0xBB, 0xB6, 0xE3,
	0x80, 0x82 };
	

const unsigned char TestData_Str_Text[] =
//This string is used for testing all kinds of text processing algorithms.
//The length of the string is 2935 bytes (without the trail 0).
"Joy, beautiful sparkle of god,\n"
"Daughter of Elysium,\n"
"We enter, fire-drunk,\n"
"Heavenly one, your shrine.\n"
"Your magics bind again\n"
"What custom has strictly parted.\n"
"All men become brothers\n"
"Where your tender wing lingers.\n\n"

"Be embraced, millions!\n"
"This kiss to the entire world!\n"
"Brothers, above the starry canopy\n"
"Must a loving Father reside.\n\n"

"Who has succeeded in the great attempt\n"
"To be a friend's friend;\n"
"Whoever has won a lovely woman\n"
"Add in his jubilation!\n"
"Yes, who calls even one soul\n"
"His own on the earth's sphere!\n"
"And whoever never could achieve this,\n"
"Let him steal away crying from this gathering!\n\n"

"Those who occupy the great circle,\n"
"Pay homage to sympathy!\n"
"It leads to the stars\n"
"Where the unknown one reigns.\n\n"

"All creatures drink joy\n"
"At the breasts of nature,\n"
"All good, all evil\n"
"Follow her trail of roses.\n"
"Kisses she gave us, and the vine,\n"
"A friend, proven in death.\n"
"Pleasure was given to the worm,\n"
"And the cherub stands before God.\n\n"

"Do you fall down, you millions?\n"
"Do you sense the creator, world?\n"
"Seek him above the starry canopy,\n"
"Above the stars he must live.\n\n"

"Joy is the name of the strong spring\n"
"In eternal nature.\n"
"Joy, joy drives the wheels\n"
"In the great clock of worlds.\n"
"She lures flowers from the buds,\n"
"Suns out of the firmament,\n"
"She rolls spheres in the spaces\n"
"That the seer's telescope does not know.\n\n"

"Happy, as his suns fly\n"
"Across Heaven's splendid map,\n"
"Run, brothers, along your path\n"
"Joyfully, as a hero to victory.\n\n"

"From the fiery mirror of truth\n"
"She smiles upon the researcher,\n"
"Towards virtue¡¯s steep hill\n"
"She guides the endurer¡¯s path.\n"
"Upon faith¡¯s sunlit mountain\n"
"One sees her banners in the wind,\n"
"Through the opening of burst coffins\n"
"One sees them standing in the chorus of angels.\n\n"

"Endure courageously, millions!\n"
"Endure for the better world!\n"
"There above the starry canopy\n"
"A great God will reward.\n\n"

"Gods one cannot repay\n"
"Beautiful it is, to be like them.\n"
"Grief and poverty, acquaint yourselves\n"
"With the joyful ones rejoice.\n"
"Anger and revenge be forgotten,\n"
"Our deadly enemy be forgiven,\n"
"No tears shall he shed,\n"
"No remorse shall gnaw at him.\n\n"

"Our debt registers be abolished,\n"
"Reconcile the entire world!\n"
"Brothers, over the starry canopy\n"
"God judges, as we judged.\n\n"

"Joy bubbles in the cup,\n"
"In the grape¡¯s golden blood.\n"
"Cannibals drink gentleness,\n"
"The fearful, courage --\n"
"Brothers, fly from your perches,\n"
"When the full cup is passed,\n"
"Let the foam spray to the heavens,\n"
"This glass to the good spirit.\n\n"

"He whom the spirals of stars praise,\n"
"He whom the seraphim's hymn glorifies,\n"
"This glass to the good spirit\n"
"Above the starry canopy!\n\n"

"Courage firm in great suffering,\n"
"Help there, where innocence weeps,\n"
"Eternally sworn oaths,\n"
"Truth towards friend and foe,\n"
"Mens' pride before kings' thrones --\n"
"Brothers, even if it costs property and blood, --\n"
"The crowns to those who earn them,\n"
"Defeat to the lying brood!\n\n"

"Close the holy circle tighter,\n"
"Swear by this golden vine:\n"
"Remain true to the vows,\n"
"Swear by the judge above the stars!"
;
#endif