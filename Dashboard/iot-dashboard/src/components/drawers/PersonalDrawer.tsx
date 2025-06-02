'use client'
import {
    Drawer,
    DrawerClose,
    DrawerContent,
    DrawerDescription,
    DrawerFooter,
    DrawerHeader,
    DrawerTitle,
    DrawerTrigger
} from '../ui/drawer'
import { Button } from '../ui/button'
import { ProfileForm } from '../forms/ProfileForm'
import { User } from 'lucide-react'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import { useState } from 'react'
import { SettingForm } from '../forms/SettingForm'


export function PersonalDrawer({ deviceId }: { deviceId: string }) {

    const [isOpen, setIsOpen] = useState<boolean>(false)

    return (
        <Drawer open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DrawerTrigger asChild >
                <Button variant="ghost" >
                    <User className="h-4 w-4" />
                </Button>
            </DrawerTrigger>
            <DrawerContent>
                <Tabs defaultValue="profile" className="w-full">
                    <TabsList className="grid w-full grid-cols-2">
                        <TabsTrigger value="profile">View Profile</TabsTrigger>
                        <TabsTrigger value="setting">View Setting</TabsTrigger>
                    </TabsList>
                    <TabsContent value="profile">
                        {/* <SignInForm /> */}
                        <DrawerHeader className="text-left">
                            <DrawerTitle>Task List</DrawerTitle>
                            <DrawerDescription>
                                View your task list below.
                            </DrawerDescription>
                        </DrawerHeader>
                        <ProfileForm />
                    </TabsContent>
                    <TabsContent value="setting">
                        {/* <RegisterForm /> */}
                        <DrawerHeader className="text-left">
                            <DrawerTitle>Setting</DrawerTitle>
                            <DrawerDescription>
                                Here is your telemetry settings to connect to
                                ThingsBoard:
                            </DrawerDescription>
                        </DrawerHeader>
                        <SettingForm deviceId={deviceId} />
                    </TabsContent>
                </Tabs>
                <DrawerFooter className="pt-2" >
                    <DrawerClose asChild >
                        <Button variant="default" > Close </Button>
                    </DrawerClose>
                </DrawerFooter>
            </DrawerContent>
        </Drawer>
    )
}
